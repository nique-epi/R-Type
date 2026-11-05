#include "ServersController.hpp"
#include <charconv>
#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "CreatedServer.hpp"
#include "CreationAttempt.hpp"
#include "GameListing.hpp"
#include "HttpConstants.hpp"
#include "HttpHeader.hpp"
#include "KnownServer.hpp"
#include "MasterDatabase.hpp"
#include "MasterHttpConstants.hpp"
#include "MasterMessageConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "NetworkException.hpp"
#include "ServerRecord.hpp"
#include "ServerRegistration.hpp"
#include "ServersControllerConstants.hpp"
#include "ServersService.hpp"

namespace rtype::master {

namespace {

std::chrono::sys_seconds now() {
  return std::chrono::floor<std::chrono::seconds>(
      std::chrono::system_clock::now());
}

ApiResponse refusal(int status, std::string_view detail) {
  return ApiResponse{
      .status = status, .body = {}, .detail = std::string{detail}};
}

ApiResponse unauthorizedResponse() {
  return ApiResponse{
      .status = UNAUTHORIZED_STATUS,
      .body = {},
      .detail = std::string{UNKNOWN_KEY_DETAIL},
      .header = HttpHeader{.name = std::string{WWW_AUTHENTICATE_HEADER},
                           .value = std::string{network::BEARER_SCHEME}}};
}

/** @returns The id the path of @p request names, nothing when it is no id. */
std::optional<std::int64_t> serverIdOf(const ApiRequest& request) {
  const auto parameter = request.pathParameters.find(
      std::string{network::SERVER_ID_PATH_PARAMETER});
  if (parameter == request.pathParameters.end()) {
    return std::nullopt;
  }
  const std::string& text = parameter->second;
  std::int64_t serverId = 0;
  const auto [end, error] =
      std::from_chars(text.data(), text.data() + text.size(), serverId);
  if (error != std::errc{} || end != text.data() + text.size() ||
      std::cmp_less(serverId, network::FIRST_SERVER_ID)) {
    return std::nullopt;
  }
  return serverId;
}

/** @returns What @p answer gives, or 400 naming the member it refused. */
template <typename Answer>
ApiResponse answerOrRefuse(Answer answer) {
  try {
    return answer();
  } catch (const network::InvalidMasterMessageException& invalid) {
    return refusal(BAD_REQUEST_STATUS, invalid.what());
  }
}

/**
 * @returns What @p answer gives for the server of the key of @p request: 401
 * when no server has that key, checked before the body is read, or 400 naming
 * the member @p answer refused.
 */
template <typename Answer>
ApiResponse answerAsServerOfKey(ServersService& service,
                                const ApiRequest& request, Answer answer) {
  const std::optional<KnownServer> server =
      service.serverOfKey(request.bearerToken.value_or(std::string{}));
  if (!server.has_value()) {
    return unauthorizedResponse();
  }
  return answerOrRefuse([&answer, &server] { return answer(*server); });
}

ApiResponse createdResponse(const network::CreatedServer& created) {
  return ApiResponse{
      .status = CREATED_STATUS,
      .body = network::createdServerText(created),
      .detail = std::nullopt,
      .header = HttpHeader{.name = std::string{LOCATION_HEADER},
                           .value = network::memberPathOf(
                               network::CREATE_SERVER_ROUTE, created.id)}};
}

/** @returns The registration @p request sends, its address always known. */
network::ServerRegistration registrationOf(const ApiRequest& request) {
  network::ServerRegistration registration =
      network::registrationFromText(request.body);
  if (!registration.address.has_value()) {
    registration.address = request.remoteAddress;
  }
  return registration;
}

ApiResponse serverResponse(std::int64_t serverId,
                           const network::ServerRegistration& registration) {
  nlohmann::json server =
      nlohmann::json::parse(network::registrationText(registration));
  server[network::ID_MEMBER] = serverId;
  return ApiResponse{.status = OK_STATUS, .body = server.dump()};
}

ApiResponse heartbeatAnswer() {
  nlohmann::json answer;
  answer[network::COMMANDS_MEMBER] = nlohmann::json::array();
  return ApiResponse{.status = OK_STATUS, .body = answer.dump()};
}

}  // namespace

ServersController::ServersController(MasterDatabase& database)
    : service_(database) {}

ApiResponse ServersController::createServer(const ApiRequest& request) {
  return answerOrRefuse([this, &request] {
    const std::optional<network::CreatedServer> created =
        service_.createServer(CreationAttempt{
            .creation = network::serverCreationFromText(request.body),
            .address = request.remoteAddress,
            .requestedAt = now()});
    if (!created.has_value()) {
      return refusal(TOO_MANY_REQUESTS_STATUS, CREATION_QUOTA_DETAIL);
    }
    return createdResponse(*created);
  });
}

ApiResponse ServersController::replaceOwnServer(const ApiRequest& request) {
  return answerAsServerOfKey(
      service_, request, [this, &request](const KnownServer& server) {
        const network::ServerRegistration registration =
            registrationOf(request);
        service_.replaceOwnServer(ServerRecord{.serverId = server.id,
                                               .registration = registration,
                                               .registeredAt = now()});
        return serverResponse(server.id, registration);
      });
}

ApiResponse ServersController::replaceOwnHeartbeat(const ApiRequest& request) {
  return answerAsServerOfKey(
      service_, request, [this, &request](const KnownServer& server) {
        if (!service_.replaceOwnHeartbeat(
                server, network::heartbeatFromText(request.body), now())) {
          return refusal(CONFLICT_STATUS, UNREGISTERED_SERVER_DETAIL);
        }
        return heartbeatAnswer();
      });
}

ApiResponse ServersController::listGames(const ApiRequest& request) {
  const std::optional<std::int64_t> serverId = serverIdOf(request);
  if (!serverId.has_value()) {
    return refusal(NOT_FOUND_STATUS, UNKNOWN_SERVER_DETAIL);
  }
  const std::optional<network::GameListing> listing =
      service_.gameListingOf(*serverId);
  if (!listing.has_value()) {
    return refusal(NOT_FOUND_STATUS, UNKNOWN_SERVER_DETAIL);
  }
  return ApiResponse{.status = OK_STATUS,
                     .body = network::gameListingText(*listing)};
}

}  // namespace rtype::master

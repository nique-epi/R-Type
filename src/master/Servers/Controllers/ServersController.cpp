#include "ServersController.hpp"
#include <chrono>
#include <cstdint>
#include <functional>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "CreatedServer.hpp"
#include "CreationAttempt.hpp"
#include "GameListing.hpp"
#include "HttpConstants.hpp"
#include "HttpHeader.hpp"
#include "IProbeSender.hpp"
#include "KnownServer.hpp"
#include "MasterDatabase.hpp"
#include "MasterHttpConstants.hpp"
#include "MasterMessageConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "NetworkException.hpp"
#include "OutgoingDatagram.hpp"
#include "PageRequest.hpp"
#include "ServerRecord.hpp"
#include "ServerRegistration.hpp"
#include "ServersControllerConstants.hpp"
#include "ServersService.hpp"
#include "WholeNumberText.hpp"

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
  const std::optional<std::uint64_t> serverId =
      network::wholeNumberIn<std::uint64_t>(parameter->second);
  if (!serverId.has_value() || *serverId < network::FIRST_SERVER_ID ||
      *serverId > network::HIGHEST_SERVER_ID) {
    return std::nullopt;
  }
  return static_cast<std::int64_t>(*serverId);
}

/** @brief The values a parameter of a query may take, and the one it takes
 * when the request leaves it out. */
struct QueryBounds {
  std::int64_t lowest{};
  std::int64_t highest{};
  std::int64_t absent{};
};

/**
 * @returns The parameter @p name of the query of @p request: bounds.absent
 * when the request leaves it out, nothing when it is no whole number from
 * bounds.lowest to bounds.highest.
 */
std::optional<std::int64_t> queryNumberOf(const ApiRequest& request,
                                          std::string_view name,
                                          QueryBounds bounds) {
  const auto parameter = request.queryParameters.find(std::string{name});
  if (parameter == request.queryParameters.end()) {
    return bounds.absent;
  }
  const std::optional<std::uint64_t> number =
      network::wholeNumberIn<std::uint64_t>(parameter->second);
  if (!number.has_value() || std::cmp_less(*number, bounds.lowest) ||
      std::cmp_greater(*number, bounds.highest)) {
    return std::nullopt;
  }
  return static_cast<std::int64_t>(*number);
}

/** @returns The page the query of @p request asks for, nothing when its limit
 * or its offset is out of its bounds. */
std::optional<PageRequest> pageOf(const ApiRequest& request) {
  const std::optional<std::int64_t> limit =
      queryNumberOf(request, network::LIMIT_QUERY_PARAMETER,
                    QueryBounds{.lowest = network::MINIMUM_PAGE_SIZE,
                                .highest = network::MAXIMUM_PAGE_SIZE,
                                .absent = network::DEFAULT_PAGE_SIZE});
  const std::optional<std::int64_t> offset =
      queryNumberOf(request, network::OFFSET_QUERY_PARAMETER,
                    QueryBounds{.lowest = network::FIRST_PAGE_OFFSET,
                                .highest = network::HIGHEST_PAGE_OFFSET,
                                .absent = network::FIRST_PAGE_OFFSET});
  if (!limit.has_value() || !offset.has_value()) {
    return std::nullopt;
  }
  return PageRequest{.limit = *limit, .offset = *offset};
}

/** @returns The answer of a page out of its bounds: 400, whose detail gives
 * the bounds. */
ApiResponse invalidPageRefusal() {
  return refusal(BAD_REQUEST_STATUS,
                 std::string{INVALID_LIMIT_DETAIL_START} +
                     std::to_string(network::MINIMUM_PAGE_SIZE) +
                     std::string{DETAIL_RANGE_JOIN} +
                     std::to_string(network::MAXIMUM_PAGE_SIZE) +
                     std::string{INVALID_OFFSET_DETAIL_START} +
                     std::to_string(network::FIRST_PAGE_OFFSET));
}

/**
 * @returns The answer of a route that reads a page of the history of the
 * server its path names: 404 for an id that names no server, 400 for a page
 * out of its bounds, else 200 and the text @p write gives of what @p read
 * found.
 */
template <typename Read, typename Write>
ApiResponse answerOfServerPage(const ApiRequest& request, Read read,
                               Write write) {
  const std::optional<std::int64_t> serverId = serverIdOf(request);
  if (!serverId.has_value()) {
    return refusal(NOT_FOUND_STATUS, UNKNOWN_SERVER_DETAIL);
  }
  const std::optional<PageRequest> page = pageOf(request);
  if (!page.has_value()) {
    return invalidPageRefusal();
  }
  const auto listing = read(*serverId, *page);
  if (!listing.has_value()) {
    return refusal(NOT_FOUND_STATUS, UNKNOWN_SERVER_DETAIL);
  }
  return ApiResponse{.status = OK_STATUS, .body = write(*listing)};
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

ServersController::ServersController(MasterDatabase& database,
                                     IProbeSender& probeSender)
    : service_(database), probeSender_(probeSender) {}

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
        const std::chrono::sys_seconds receivedAt = now();
        if (!service_.replaceOwnHeartbeat(
                server, network::heartbeatFromText(request.body), receivedAt)) {
          return refusal(CONFLICT_STATUS, UNREGISTERED_SERVER_DETAIL);
        }
        if (std::optional<network::OutgoingDatagram> probe =
                service_.probeFor(server, receivedAt)) {
          probeSender_.sendProbe(std::move(*probe));
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

ApiResponse ServersController::listServers(const ApiRequest& request) {
  const std::optional<PageRequest> page = pageOf(request);
  if (!page.has_value()) {
    return invalidPageRefusal();
  }
  return ApiResponse{
      .status = OK_STATUS,
      .body = network::serverListingText(service_.serverListingOf(*page))};
}

ApiResponse ServersController::listSamples(const ApiRequest& request) {
  return answerOfServerPage(
      request, std::bind_front(&ServersService::sampleListingOf, &service_),
      network::sampleListingText);
}

ApiResponse ServersController::listStatuses(const ApiRequest& request) {
  return answerOfServerPage(
      request, std::bind_front(&ServersService::statusListingOf, &service_),
      network::statusListingText);
}

}  // namespace rtype::master

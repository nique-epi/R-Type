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
#include "MasterMessageConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "OutgoingDatagram.hpp"
#include "PageRequest.hpp"
#include "RouteAnswers.hpp"
#include "ServerRecord.hpp"
#include "ServerRegistration.hpp"
#include "ServersControllerConstants.hpp"
#include "ServersService.hpp"
#include "SettingValue.hpp"
#include "SiteClaim.hpp"
#include "SiteVisitorAddress.hpp"
#include "WholeNumberText.hpp"

namespace rtype::master {

namespace {

std::chrono::sys_seconds now() {
  return std::chrono::floor<std::chrono::seconds>(
      std::chrono::system_clock::now());
}

/** @returns The id the path of @p request names, nothing when it is no id. */
std::optional<std::int64_t> serverIdOf(const network::ApiRequest& request) {
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

/**
 * @returns The answer of a route that reads a page of the history of the
 * server its path names: 404 for an id that names no server, 400 for a page
 * out of its bounds, else 200 and the text @p write gives of what @p read
 * found.
 */
template <typename Read, typename Write>
network::ApiResponse answerOfServerPage(const network::ApiRequest& request,
                                        Read read, Write write) {
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
  return network::ApiResponse{.status = OK_STATUS, .body = write(*listing)};
}

/** @returns The value of the header @p name of @p request, empty without
 * that header. */
std::string headerOf(const network::ApiRequest& request,
                     std::string_view name) {
  const auto header = request.headers.find(std::string{name});
  return header == request.headers.end() ? std::string{} : header->second;
}

/** @returns What @p request says of the web site and its visitor. */
SiteClaim siteClaimOf(const network::ApiRequest& request) {
  return SiteClaim{
      .key = headerOf(request, SITE_KEY_HEADER),
      .clientAddress = headerOf(request, SITE_CLIENT_ADDRESS_HEADER)};
}

network::ApiResponse createdResponse(const network::CreatedServer& created) {
  return network::ApiResponse{
      .status = CREATED_STATUS,
      .body = network::createdServerText(created),
      .detail = std::nullopt,
      .header =
          network::HttpHeader{.name = std::string{LOCATION_HEADER},
                              .value = network::memberPathOf(
                                  network::CREATE_SERVER_ROUTE, created.id)}};
}

/** @returns The registration @p request sends, its address always known. */
network::ServerRegistration registrationOf(const network::ApiRequest& request) {
  network::ServerRegistration registration =
      network::registrationFromText(request.body);
  if (!registration.address.has_value()) {
    registration.address = request.remoteAddress;
  }
  return registration;
}

network::ApiResponse serverResponse(
    std::int64_t serverId, const network::ServerRegistration& registration) {
  nlohmann::json server =
      nlohmann::json::parse(network::registrationText(registration));
  server[network::ID_MEMBER] = serverId;
  return network::ApiResponse{.status = OK_STATUS, .body = server.dump()};
}

network::ApiResponse heartbeatAnswer() {
  nlohmann::json answer;
  answer[network::COMMANDS_MEMBER] = nlohmann::json::array();
  return network::ApiResponse{.status = OK_STATUS, .body = answer.dump()};
}

}  // namespace

ServersController::ServersController(ServersService& service,
                                     IProbeSender& probeSender,
                                     const engine::SettingValue& siteKey)
    : service_(service), probeSender_(probeSender), siteKey_(siteKey) {}

network::ApiResponse ServersController::createServer(
    const network::ApiRequest& request) {
  return answerOrRefuse([this, &request] {
    const std::optional<network::CreatedServer> created =
        service_.createServer(CreationAttempt{
            .creation = network::serverCreationFromText(request.body),
            .address = visitorAddressOf(siteClaimOf(request), siteKey_.text())
                           .value_or(request.remoteAddress),
            .requestedAt = now()});
    if (!created.has_value()) {
      return refusal(TOO_MANY_REQUESTS_STATUS, CREATION_QUOTA_DETAIL);
    }
    return createdResponse(*created);
  });
}

network::ApiResponse ServersController::replaceOwnServer(
    const network::ApiRequest& request) {
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

network::ApiResponse ServersController::replaceOwnHeartbeat(
    const network::ApiRequest& request) {
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

network::ApiResponse ServersController::listGames(
    const network::ApiRequest& request) {
  const std::optional<std::int64_t> serverId = serverIdOf(request);
  if (!serverId.has_value()) {
    return refusal(NOT_FOUND_STATUS, UNKNOWN_SERVER_DETAIL);
  }
  const std::optional<network::GameListing> listing =
      service_.gameListingOf(*serverId);
  if (!listing.has_value()) {
    return refusal(NOT_FOUND_STATUS, UNKNOWN_SERVER_DETAIL);
  }
  return network::ApiResponse{.status = OK_STATUS,
                              .body = network::gameListingText(*listing)};
}

network::ApiResponse ServersController::listServers(
    const network::ApiRequest& request) {
  const std::optional<PageRequest> page = pageOf(request);
  if (!page.has_value()) {
    return invalidPageRefusal();
  }
  return network::ApiResponse{
      .status = OK_STATUS,
      .body = network::serverListingText(service_.serverListingOf(*page))};
}

network::ApiResponse ServersController::listSamples(
    const network::ApiRequest& request) {
  return answerOfServerPage(
      request, std::bind_front(&ServersService::sampleListingOf, &service_),
      network::sampleListingText);
}

network::ApiResponse ServersController::listStatuses(
    const network::ApiRequest& request) {
  return answerOfServerPage(
      request, std::bind_front(&ServersService::statusListingOf, &service_),
      network::statusListingText);
}

}  // namespace rtype::master

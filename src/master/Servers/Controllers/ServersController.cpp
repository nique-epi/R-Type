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
#include "QueryBounds.hpp"
#include "RouteAnswers.hpp"
#include "ServerRecord.hpp"
#include "ServerRegistration.hpp"
#include "ServersControllerConstants.hpp"
#include "ServersService.hpp"
#include "SiteVisitors.hpp"

namespace rtype::master {

namespace {

/** @returns The id the path of @p request names, nothing when it is no id. */
std::optional<std::int64_t> serverIdOf(const network::ApiRequest& request) {
  return pathNumberOf(
      request, network::SERVER_ID_PATH_PARAMETER,
      QueryBounds{
          .lowest = static_cast<std::int64_t>(network::FIRST_SERVER_ID),
          .highest = static_cast<std::int64_t>(network::HIGHEST_SERVER_ID),
          .absent = static_cast<std::int64_t>(network::FIRST_SERVER_ID)});
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
  return okResponse(write(*listing));
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
  return okResponse(server.dump());
}

network::ApiResponse heartbeatAnswer() {
  nlohmann::json answer;
  answer[network::COMMANDS_MEMBER] = nlohmann::json::array();
  return okResponse(answer.dump());
}

}  // namespace

ServersController::ServersController(ServersService& service,
                                     IProbeSender& probeSender,
                                     const SiteVisitors& siteVisitors)
    : service_(service),
      probeSender_(probeSender),
      siteVisitors_(siteVisitors) {}

network::ApiResponse ServersController::createServer(
    const network::ApiRequest& request) {
  return answerOrRefuse([this, &request] {
    const std::optional<network::CreatedServer> created =
        service_.createServer(CreationAttempt{
            .creation = network::serverCreationFromText(request.body),
            .address = siteVisitors_.addressOf(request),
            .requestedAt = currentSecond()});
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
        service_.replaceOwnServer(
            ServerRecord{.serverId = server.id,
                         .registration = registration,
                         .registeredAt = currentSecond()});
        return serverResponse(server.id, registration);
      });
}

network::ApiResponse ServersController::replaceOwnHeartbeat(
    const network::ApiRequest& request) {
  return answerAsServerOfKey(
      service_, request, [this, &request](const KnownServer& server) {
        const std::chrono::sys_seconds receivedAt = currentSecond();
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

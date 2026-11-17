#include "DeviceLinksController.hpp"
#include <chrono>
#include <optional>
#include <string>
#include <variant>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "CreatedDeviceLink.hpp"
#include "DeviceLinkRefusal.hpp"
#include "DeviceLinkState.hpp"
#include "DeviceLinkVerdict.hpp"
#include "DeviceLinksControllerConstants.hpp"
#include "DeviceLinksService.hpp"
#include "DeviceLinksServiceConstants.hpp"
#include "HttpConstants.hpp"
#include "HttpHeader.hpp"
#include "ISessionAccounts.hpp"
#include "MasterMessageText.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "RouteAnswers.hpp"
#include "SiteVisitors.hpp"
#include "User.hpp"

namespace rtype::master {

namespace {

/** @returns The secret of a device link @p request carries as Bearer, nothing
 * when it carries none of that shape. */
std::optional<std::string> secretOf(const network::ApiRequest& request) {
  if (!request.bearerToken.has_value() ||
      !network::isWellFormedDeviceLinkSecret(*request.bearerToken)) {
    return std::nullopt;
  }
  return request.bearerToken;
}

/** @returns The short code the path of @p request names, written as the
 * master keeps it; nothing when it names no short code. */
std::optional<std::string> userCodeOf(const network::ApiRequest& request) {
  const std::optional<std::string> text =
      pathTextOf(request, network::DEVICE_LINK_CODE_PATH_PARAMETER);
  if (!text.has_value()) {
    return std::nullopt;
  }
  return network::deviceLinkUserCodeOf(*text);
}

network::ApiResponse createdLinkResponse(
    const network::CreatedDeviceLink& link) {
  return network::ApiResponse{
      .status = CREATED_STATUS,
      .body = network::createdDeviceLinkText(link),
      .detail = std::nullopt,
      .header = network::HttpHeader{
          .name = std::string{LOCATION_HEADER},
          .value = network::apiPathOf(network::READ_OWN_DEVICE_LINK_ROUTE)}};
}

/** @returns 429 for a read before the interval, with `Retry-After` giving the
 * seconds to wait, as RFC 9110 (section 10.2.3) writes them. */
network::ApiResponse tooSoonResponse() {
  network::ApiResponse response =
      refusal(TOO_MANY_REQUESTS_STATUS, READ_TOO_SOON_DETAIL);
  response.header = network::HttpHeader{
      .name = std::string{RETRY_AFTER_HEADER},
      .value = std::to_string(DEVICE_LINK_INTERVAL.count())};
  return response;
}

/** @returns The answer of a read of its own link by the game: its state, or
 * why it is refused. */
network::ApiResponse ownStateResponse(
    const std::variant<network::DeviceLinkState, DeviceLinkRefusal>& read) {
  if (const auto* state = std::get_if<network::DeviceLinkState>(&read)) {
    return okResponse(network::deviceLinkStateText(*state));
  }
  if (std::get<DeviceLinkRefusal>(read) == DeviceLinkRefusal::TooSoon) {
    return tooSoonResponse();
  }
  return unauthorizedResponse(NO_DEVICE_LINK_DETAIL);
}

/** @returns The answer of a decision of a player: the state of the link, or
 * why the decision is refused. */
network::ApiResponse decisionResponse(
    const std::variant<network::DeviceLinkState, DeviceLinkRefusal>& decided) {
  if (const auto* state = std::get_if<network::DeviceLinkState>(&decided)) {
    return okResponse(network::deviceLinkStateText(*state));
  }
  if (std::get<DeviceLinkRefusal>(decided) == DeviceLinkRefusal::Unknown) {
    return refusal(NOT_FOUND_STATUS, UNKNOWN_USER_CODE_DETAIL);
  }
  return refusal(CONFLICT_STATUS, ALREADY_DECIDED_DETAIL);
}

}  // namespace

DeviceLinksController::DeviceLinksController(DeviceLinksService& links,
                                             ISessionAccounts& accounts,
                                             const SiteVisitors& siteVisitors)
    : links_(links), accounts_(accounts), siteVisitors_(siteVisitors) {}

network::ApiResponse DeviceLinksController::createDeviceLink(
    const network::ApiRequest& request) {
  const std::variant<network::CreatedDeviceLink, DeviceLinkRefusal> created =
      links_.create(siteVisitors_.addressOf(request), currentSecond());
  if (const auto* link = std::get_if<network::CreatedDeviceLink>(&created)) {
    return createdLinkResponse(*link);
  }
  return refusal(TOO_MANY_REQUESTS_STATUS, DEVICE_LINK_QUOTA_DETAIL);
}

network::ApiResponse DeviceLinksController::readOwnDeviceLink(
    const network::ApiRequest& request) {
  const std::optional<std::string> secret = secretOf(request);
  if (!secret.has_value()) {
    return unauthorizedResponse(NO_DEVICE_LINK_DETAIL);
  }
  return ownStateResponse(links_.readOwn(*secret, currentSecond()));
}

network::ApiResponse DeviceLinksController::deleteOwnDeviceLink(
    const network::ApiRequest& request) {
  const std::optional<std::string> secret = secretOf(request);
  if (!secret.has_value()) {
    return unauthorizedResponse(NO_DEVICE_LINK_DETAIL);
  }
  links_.forgetOwn(*secret);
  return network::ApiResponse{.status = NO_CONTENT_STATUS, .body = {}};
}

network::ApiResponse DeviceLinksController::readDeviceLink(
    const network::ApiRequest& request) {
  return answerAsAccountOf(
      accounts_, request,
      [this, &request](const network::User& /*user*/,
                       std::chrono::sys_seconds now) {
        const std::optional<std::string> userCode = userCodeOf(request);
        const std::optional<network::DeviceLinkState> state =
            userCode.has_value() ? links_.read(*userCode, now) : std::nullopt;
        if (!state.has_value()) {
          return refusal(NOT_FOUND_STATUS, UNKNOWN_USER_CODE_DETAIL);
        }
        return okResponse(network::deviceLinkStateText(*state));
      });
}

network::ApiResponse DeviceLinksController::replaceDeviceLinkDecision(
    const network::ApiRequest& request) {
  return answerAsAccountOf(
      accounts_, request,
      [this, &request](const network::User& user,
                       std::chrono::sys_seconds now) {
        const DeviceLinkVerdict verdict{
            .userId = user.id,
            .decision = network::decisionMessageFromText(request.body)};
        const std::optional<std::string> userCode = userCodeOf(request);
        if (!userCode.has_value()) {
          return refusal(NOT_FOUND_STATUS, UNKNOWN_USER_CODE_DETAIL);
        }
        return decisionResponse(links_.decide(*userCode, verdict, now));
      });
}

}  // namespace rtype::master

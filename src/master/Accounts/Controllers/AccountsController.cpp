#include "AccountsController.hpp"
#include <chrono>
#include <optional>
#include <string>
#include <variant>
#include "AccountsControllerConstants.hpp"
#include "AccountsService.hpp"
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "CreatedSession.hpp"
#include "CredentialsAttempt.hpp"
#include "CredentialsRefusal.hpp"
#include "DeviceLinkRefusal.hpp"
#include "DeviceLinksControllerConstants.hpp"
#include "DeviceLinksService.hpp"
#include "HttpConstants.hpp"
#include "HttpHeader.hpp"
#include "MasterMessageText.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "RouteAnswers.hpp"
#include "ServersControllerConstants.hpp"
#include "SessionOrigin.hpp"
#include "SiteVisitors.hpp"
#include "User.hpp"

namespace rtype::master {

namespace {

/** @returns What @p request asks, counted under the visitor of the web site
 * it names, or the client it came from. */
CredentialsAttempt credentialsAttemptOf(const network::ApiRequest& request,
                                        const SiteVisitors& siteVisitors) {
  return CredentialsAttempt{
      .credentials = network::userCredentialsFromText(request.body),
      .address = siteVisitors.addressOf(request),
      .requestedAt = currentSecond()};
}

network::ApiResponse refusalOf(CredentialsRefusal reason) {
  switch (reason) {
    case CredentialsRefusal::NicknameTaken:
      return refusal(CONFLICT_STATUS, NICKNAME_TAKEN_DETAIL);
    case CredentialsRefusal::TooManyCreations:
      return refusal(TOO_MANY_REQUESTS_STATUS, USER_QUOTA_DETAIL);
    case CredentialsRefusal::WrongCredentials:
      return unauthorizedResponse(WRONG_CREDENTIALS_DETAIL);
    case CredentialsRefusal::TooManyFailures:
      return refusal(TOO_MANY_REQUESTS_STATUS, SIGN_IN_QUOTA_DETAIL);
  }
  return unauthorizedResponse(WRONG_CREDENTIALS_DETAIL);
}

/** @returns What @p answer writes of the value of @p outcome, or the refusal
 * it holds. */
template <typename Value, typename Answer>
network::ApiResponse answerOf(
    const std::variant<Value, CredentialsRefusal>& outcome, Answer answer) {
  if (const auto* value = std::get_if<Value>(&outcome)) {
    return answer(*value);
  }
  return refusalOf(std::get<CredentialsRefusal>(outcome));
}

/** @returns Why a device link opened no session, as a problem document: 401
 * for no link of that secret, 409 while it waits, 403 once refused, 410 once
 * expired. */
network::ApiResponse linkRefusalResponse(DeviceLinkRefusal reason) {
  switch (reason) {
    case DeviceLinkRefusal::Pending:
      return refusal(CONFLICT_STATUS, LINK_PENDING_DETAIL);
    case DeviceLinkRefusal::Denied:
      return refusal(FORBIDDEN_STATUS, LINK_DENIED_DETAIL);
    case DeviceLinkRefusal::Expired:
      return refusal(GONE_STATUS, LINK_EXPIRED_DETAIL);
    case DeviceLinkRefusal::TooManyCreations:
    case DeviceLinkRefusal::Unknown:
    case DeviceLinkRefusal::TooSoon:
    case DeviceLinkRefusal::AlreadyDecided:
      break;
  }
  return unauthorizedResponse(NO_DEVICE_LINK_DETAIL);
}

/** @returns What @p answer writes of the session @p outcome opened, or why
 * the device link opened none. */
template <typename Answer>
network::ApiResponse answerOf(
    const std::variant<network::CreatedSession, DeviceLinkRefusal>& outcome,
    Answer answer) {
  if (const auto* session = std::get_if<network::CreatedSession>(&outcome)) {
    return answer(*session);
  }
  return linkRefusalResponse(std::get<DeviceLinkRefusal>(outcome));
}

network::ApiResponse createdUserResponse(const network::User& user) {
  return network::ApiResponse{
      .status = CREATED_STATUS,
      .body = network::userText(user),
      .detail = std::nullopt,
      .header = network::HttpHeader{
          .name = std::string{LOCATION_HEADER},
          .value = network::memberPathOf(network::CREATE_USER_ROUTE, user.id)}};
}

network::ApiResponse createdSessionResponse(
    const network::CreatedSession& session) {
  return network::ApiResponse{
      .status = CREATED_STATUS,
      .body = network::createdSessionText(session),
      .detail = std::nullopt,
      .header = network::HttpHeader{
          .name = std::string{LOCATION_HEADER},
          .value = network::apiPathOf(network::DELETE_OWN_SESSION_ROUTE)}};
}

/** @returns The secret of the device link @p request trades for a session:
 * a request with no body and the secret of a link as Bearer; nothing for any
 * other request. */
std::optional<std::string> tradedSecretOf(const network::ApiRequest& request) {
  if (!request.body.empty() || !request.bearerToken.has_value() ||
      !network::isWellFormedDeviceLinkSecret(*request.bearerToken)) {
    return std::nullopt;
  }
  return request.bearerToken;
}

}  // namespace

AccountsController::AccountsController(AccountsService& service,
                                       DeviceLinksService& links,
                                       const SiteVisitors& siteVisitors)
    : service_(service), links_(links), siteVisitors_(siteVisitors) {}

network::ApiResponse AccountsController::createUser(
    const network::ApiRequest& request) {
  return answerOrRefuse([this, &request] {
    return answerOf(
        service_.createUser(credentialsAttemptOf(request, siteVisitors_)),
        createdUserResponse);
  });
}

network::ApiResponse AccountsController::readOwnUser(
    const network::ApiRequest& request) {
  return answerAsAccountOf(
      service_, request,
      [](const network::User& user, std::chrono::sys_seconds /*now*/) {
        return okResponse(network::userText(user));
      });
}

network::ApiResponse AccountsController::createSession(
    const network::ApiRequest& request) {
  if (const std::optional<std::string> secret = tradedSecretOf(request)) {
    return answerOf(
        links_.openSession(
            *secret, SessionOrigin{.address = siteVisitors_.addressOf(request),
                                   .openedAt = currentSecond()}),
        createdSessionResponse);
  }
  return answerOrRefuse([this, &request] {
    return answerOf(
        service_.createSession(credentialsAttemptOf(request, siteVisitors_)),
        createdSessionResponse);
  });
}

network::ApiResponse AccountsController::deleteOwnSession(
    const network::ApiRequest& request) {
  if (!request.bearerToken.has_value()) {
    return unauthorizedResponse(NO_SESSION_DETAIL);
  }
  service_.deleteSession(*request.bearerToken);
  return network::ApiResponse{.status = NO_CONTENT_STATUS, .body = {}};
}

}  // namespace rtype::master

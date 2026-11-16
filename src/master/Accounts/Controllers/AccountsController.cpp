#include "AccountsController.hpp"
#include <chrono>
#include <functional>
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
#include "DatedRequest.hpp"
#include "HttpConstants.hpp"
#include "HttpHeader.hpp"
#include "MasterMessageText.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "RouteAnswers.hpp"
#include "ServersControllerConstants.hpp"
#include "User.hpp"

namespace rtype::master {

namespace {

std::chrono::sys_seconds now() {
  return std::chrono::floor<std::chrono::seconds>(
      std::chrono::system_clock::now());
}

CredentialsAttempt credentialsAttemptOf(const network::ApiRequest& request) {
  return CredentialsAttempt{
      .credentials = network::userCredentialsFromText(request.body),
      .address = request.remoteAddress,
      .requestedAt = now()};
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

}  // namespace

AccountsController::AccountsController(AccountsService& service)
    : service_(service) {}

network::ApiResponse AccountsController::createUser(
    const network::ApiRequest& request) {
  return answerOrRefuse([this, &request] {
    return answerOf(service_.createUser(credentialsAttemptOf(request)),
                    createdUserResponse);
  });
}

network::ApiResponse AccountsController::readOwnUser(
    const network::ApiRequest& request) {
  return answerAsAccountOf(
      service_, DatedRequest{.request = std::cref(request), .now = now()},
      [](const network::User& user) {
        return network::ApiResponse{.status = OK_STATUS,
                                    .body = network::userText(user)};
      });
}

network::ApiResponse AccountsController::createSession(
    const network::ApiRequest& request) {
  return answerOrRefuse([this, &request] {
    return answerOf(service_.createSession(credentialsAttemptOf(request)),
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

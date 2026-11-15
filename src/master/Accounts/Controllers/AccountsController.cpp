#include "AccountsController.hpp"
#include <chrono>
#include <optional>
#include <string>
#include <variant>
#include "Account.hpp"
#include "AccountRefusal.hpp"
#include "AccountsControllerConstants.hpp"
#include "AccountsService.hpp"
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "CredentialsRequest.hpp"
#include "HttpConstants.hpp"
#include "HttpHeader.hpp"
#include "MasterMessageText.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "OpenedSession.hpp"
#include "RouteAnswers.hpp"

namespace rtype::master {

namespace {

std::chrono::sys_seconds now() {
  return std::chrono::floor<std::chrono::seconds>(
      std::chrono::system_clock::now());
}

CredentialsRequest credentialsRequestOf(const network::ApiRequest& request) {
  return CredentialsRequest{
      .credentials = network::credentialsFromText(request.body),
      .address = request.remoteAddress,
      .requestedAt = now()};
}

network::ApiResponse refusalOf(AccountRefusal reason) {
  switch (reason) {
    case AccountRefusal::NicknameTaken:
      return refusal(CONFLICT_STATUS, NICKNAME_TAKEN_DETAIL);
    case AccountRefusal::TooManyCreations:
      return refusal(TOO_MANY_REQUESTS_STATUS, ACCOUNT_QUOTA_DETAIL);
    case AccountRefusal::WrongCredentials:
      return unauthorizedResponse(WRONG_CREDENTIALS_DETAIL);
    case AccountRefusal::TooManyFailures:
      return refusal(TOO_MANY_REQUESTS_STATUS, SIGN_IN_QUOTA_DETAIL);
  }
  return unauthorizedResponse(WRONG_CREDENTIALS_DETAIL);
}

/** @returns What @p answer writes of the value of @p outcome, or the refusal
 * it holds. */
template <typename Value, typename Answer>
network::ApiResponse answerOf(
    const std::variant<Value, AccountRefusal>& outcome, Answer answer) {
  if (const auto* value = std::get_if<Value>(&outcome)) {
    return answer(*value);
  }
  return refusalOf(std::get<AccountRefusal>(outcome));
}

network::ApiResponse createdAccountResponse(const network::Account& account) {
  return network::ApiResponse{
      .status = CREATED_STATUS,
      .body = network::accountText(account),
      .detail = std::nullopt,
      .header =
          network::HttpHeader{.name = std::string{LOCATION_HEADER},
                              .value = network::memberPathOf(
                                  network::CREATE_ACCOUNT_ROUTE, account.id)}};
}

network::ApiResponse openedSessionResponse(
    const network::OpenedSession& session) {
  return network::ApiResponse{
      .status = CREATED_STATUS,
      .body = network::openedSessionText(session),
      .detail = std::nullopt,
      .header = network::HttpHeader{
          .name = std::string{LOCATION_HEADER},
          .value = network::apiPathOf(network::CLOSE_OWN_SESSION_ROUTE)}};
}

}  // namespace

AccountsController::AccountsController(AccountsService& service)
    : service_(service) {}

network::ApiResponse AccountsController::createAccount(
    const network::ApiRequest& request) {
  return answerOrRefuse([this, &request] {
    return answerOf(service_.createAccount(credentialsRequestOf(request)),
                    createdAccountResponse);
  });
}

network::ApiResponse AccountsController::readOwnAccount(
    const network::ApiRequest& request) {
  const std::optional<network::Account> account = service_.accountOfToken(
      request.bearerToken.value_or(std::string{}), now());
  if (!account.has_value()) {
    return unauthorizedResponse(NO_SESSION_DETAIL);
  }
  return network::ApiResponse{.status = OK_STATUS,
                              .body = network::accountText(*account)};
}

network::ApiResponse AccountsController::openSession(
    const network::ApiRequest& request) {
  return answerOrRefuse([this, &request] {
    return answerOf(service_.openSession(credentialsRequestOf(request)),
                    openedSessionResponse);
  });
}

network::ApiResponse AccountsController::closeOwnSession(
    const network::ApiRequest& request) {
  if (!request.bearerToken.has_value()) {
    return unauthorizedResponse(NO_SESSION_DETAIL);
  }
  service_.closeSession(*request.bearerToken);
  return network::ApiResponse{.status = NO_CONTENT_STATUS, .body = {}};
}

network::ApiResponse AccountsController::closedAnswer(
    [[maybe_unused]] const network::ApiRequest& request) {
  return refusal(SERVICE_UNAVAILABLE_STATUS, ACCOUNTS_CLOSED_DETAIL);
}

}  // namespace rtype::master

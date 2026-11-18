#include "GameTicketController.hpp"
#include <chrono>
#include <optional>
#include <string>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "GameTicketControllerConstants.hpp"
#include "GameTicketGrant.hpp"
#include "GameTicketService.hpp"
#include "GameTicketState.hpp"
#include "GameTicketText.hpp"
#include "HttpConstants.hpp"
#include "HttpHeader.hpp"
#include "ISessionAccounts.hpp"
#include "MasterMessageText.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "RouteAnswers.hpp"
#include "User.hpp"

namespace rtype::master {

namespace {

/** @returns Where the ticket of @p fingerprint lives:
 * `/api/users/me/tickets/<fingerprint>`. */
std::string ticketLocationOf(const std::string& fingerprint) {
  return network::apiPathOf(network::CREATE_OWN_TICKET_ROUTE) +
         std::string{network::PATH_SEPARATOR} + fingerprint;
}

network::ApiResponse createdTicketResponse(
    const network::GameTicketGrant& grant) {
  return network::ApiResponse{
      .status = CREATED_STATUS,
      .body = network::gameTicketGrantText(grant),
      .detail = std::nullopt,
      .header = network::HttpHeader{
          .name = std::string{LOCATION_HEADER},
          .value = ticketLocationOf(gameTicketFingerprintOf(grant.ticket))}};
}

}  // namespace

GameTicketController::GameTicketController(ISessionAccounts& accounts,
                                           GameTicketService& service)
    : accounts_(accounts), service_(service) {}

network::ApiResponse GameTicketController::createOwnTicket(
    const network::ApiRequest& request) {
  return answerAsAccountOf(
      accounts_, request,
      [this](const network::User& user, std::chrono::sys_seconds now) {
        const std::optional<network::GameTicketGrant> grant =
            service_.issue(user.id, now);
        if (!grant.has_value()) {
          return refusal(TOO_MANY_REQUESTS_STATUS, TICKET_QUOTA_DETAIL);
        }
        return createdTicketResponse(*grant);
      });
}

network::ApiResponse GameTicketController::readOwnTicket(
    const network::ApiRequest& request) {
  return answerAsAccountOf(
      accounts_, request,
      [this, &request](const network::User& user,
                       std::chrono::sys_seconds /*now*/) {
        const std::optional<std::string> fingerprint =
            pathTextOf(request, network::TICKET_PATH_PARAMETER);
        const std::optional<network::GameTicketState> state =
            fingerprint.has_value() ? service_.stateOf(user.id, *fingerprint)
                                    : std::nullopt;
        if (!state.has_value()) {
          return refusal(NOT_FOUND_STATUS, UNKNOWN_TICKET_DETAIL);
        }
        return okResponse(network::gameTicketStateText(*state));
      });
}

}  // namespace rtype::master

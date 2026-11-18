#pragma once

#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "GameTicketService.hpp"
#include "ISessionAccounts.hpp"

namespace rtype::master {

/**
 * @brief The routes of the game tickets. Each is network::MasterRoute::
 * httpsOnly, so a request that did not reach the master over HTTPS gets 403
 * before it reaches the controller, and each answers the account of the
 * session of its token only: 401, with `WWW-Authenticate: Bearer`, without
 * one.
 */
class GameTicketController {
 public:
  /** @param accounts Names the account of a session; @p service gives the
   * tickets. Both must outlive the controller. */
  GameTicketController(ISessionAccounts& accounts, GameTicketService& service);

  /**
   * @returns The answer of `POST /api/users/me/tickets`: 201, `Location`
   * naming the ticket by its fingerprint, and the ticket with its expiry; 429
   * when the account was given its quota of tickets in the last hour.
   */
  [[nodiscard]] network::ApiResponse createOwnTicket(
      const network::ApiRequest& request);

  /** @returns The answer of `GET /api/users/me/tickets/{ticket}`: 200, the
   * expiry and whether a result was paid through it; 404 when the account
   * was given no ticket of that fingerprint, or it expired and was
   * forgotten. */
  [[nodiscard]] network::ApiResponse readOwnTicket(
      const network::ApiRequest& request);

 private:
  ISessionAccounts& accounts_;
  GameTicketService& service_;
};

}  // namespace rtype::master

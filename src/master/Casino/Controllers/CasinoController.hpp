#pragma once

#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "CasinoService.hpp"
#include "ISessionAccounts.hpp"

namespace rtype::master {

/**
 * @brief The routes of the casino: what reads their request and writes their
 * answer. The list of the rooms and the state of a room are open to everyone;
 * the seats and the bets are network::MasterRoute::httpsOnly, so a request
 * that did not reach the master over HTTPS gets 403 before it reaches the
 * controller, and answer the account of the session of their token only: 401,
 * with `WWW-Authenticate: Bearer`, without one. Every refusal is a problem
 * document: 400 naming the faulty member, 404 for a room the casino does not
 * run, 409 for a full room, a round whose bets are closed or an account not
 * seated, 422 for bets the rules or the wallet refuse.
 */
class CasinoController {
 public:
  /** @param accounts Names the account of a session; @p service applies the
   * rules of the casino. Both must outlive the controller. */
  CasinoController(ISessionAccounts& accounts, CasinoService& service);

  /** @returns The answer of `GET /api/casino/rooms`: 200 and every room. */
  [[nodiscard]] network::ApiResponse listRooms(
      const network::ApiRequest& request);

  /** @returns The answer of `GET /api/casino/rooms/{room}`: 200 and the state
   * of the room; 404 for a room the casino does not run. */
  [[nodiscard]] network::ApiResponse readRoom(
      const network::ApiRequest& request);

  /** @returns The answer of `PUT /api/casino/rooms/{room}/seats/me`: 201 and
   * the state of the room once seated, 200 and the same once a seat held is
   * renewed; 409 when the room is full, 404 for no such room. */
  [[nodiscard]] network::ApiResponse takeOwnSeat(
      const network::ApiRequest& request);

  /** @returns The answer of `DELETE /api/casino/rooms/{room}/seats/me`: 204,
   * whether the account was seated or not, and for any room. */
  [[nodiscard]] network::ApiResponse leaveOwnSeat(
      const network::ApiRequest& request);

  /**
   * @returns The answer of
   * `PUT /api/casino/rooms/{room}/rounds/{round}/bets/me`: 200, the bets,
   * their stake and the chips left; 409 when the bets of the round are closed
   * or the account is not seated; 422, whose detail names the rule, for bets
   * the cloth, the caps or the wallet refuse; 404 for no such room or a round
   * that is no number; 400 for a body that is no list of bets.
   */
  [[nodiscard]] network::ApiResponse placeOwnBets(
      const network::ApiRequest& request);

  /** @returns The answer of
   * `GET /api/casino/rooms/{room}/rounds/{round}/bets/me`: 200, the bets of
   * the account on the round, none when it holds none, and what they returned
   * once drawn; 404 for no such room or a round that is no number. */
  [[nodiscard]] network::ApiResponse readOwnBets(
      const network::ApiRequest& request);

 private:
  ISessionAccounts& accounts_;
  CasinoService& service_;
};

}  // namespace rtype::master

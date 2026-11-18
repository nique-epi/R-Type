#pragma once

#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "ISessionAccounts.hpp"
#include "WalletService.hpp"

namespace rtype::master {

/**
 * @brief The routes of the wallets: what reads their request and writes their
 * answer. Each route is network::MasterRoute::httpsOnly, so a request that did
 * not reach the master over HTTPS gets 403 from the MasterHttpServer before it
 * reaches the controller, and each answers the account of the session of its
 * token only: 401, with `WWW-Authenticate: Bearer`, without one. Every refusal
 * is a problem document: 400 naming the faulty member, 404 for an exchange or
 * a look the path names and none has, 409 for a balance that cannot pay, 422
 * for an amount or a look the cashier or the shop never takes.
 */
class WalletController {
 public:
  /** @param accounts Names the account of a session; @p service applies the
   * rules of the wallets. Both must outlive the controller. */
  WalletController(ISessionAccounts& accounts, WalletService& service);

  /** @returns The answer of `GET /api/users/me/wallet`: 200 and the
   * balances. */
  [[nodiscard]] network::ApiResponse readOwnWallet(
      const network::ApiRequest& request);

  /** @returns The answer of `GET /api/users/me/wallet/movements`: 200 and a
   * page of the journal, newest first; 400 for a page out of its bounds. */
  [[nodiscard]] network::ApiResponse listOwnMovements(
      const network::ApiRequest& request);

  /**
   * @returns The answer of `POST /api/users/me/wallet/exchanges`: 201,
   * `Location` naming the exchange, the exchange and the balances; 422 for an
   * amount of none or past 999999, 409 when a balance cannot take it.
   */
  [[nodiscard]] network::ApiResponse createOwnExchange(
      const network::ApiRequest& request);

  /** @returns The answer of `GET /api/users/me/wallet/exchanges/{exchange}`:
   * 200 and the exchange; 404 when the account made none of that number. */
  [[nodiscard]] network::ApiResponse readOwnExchange(
      const network::ApiRequest& request);

  /** @returns The answer of `GET /api/users/me/looks`: 200 and the looks the
   * account owns. */
  [[nodiscard]] network::ApiResponse listOwnLooks(
      const network::ApiRequest& request);

  /**
   * @returns The answer of `PUT /api/users/me/looks/{item}`: 201 and the
   * balances once bought, 200 when the account owned it already and nothing
   * was taken; 404 for a name of no look, 422 for a look the shop does not
   * sell, 409 when it costs more than the kopecks.
   */
  [[nodiscard]] network::ApiResponse buyOwnLook(
      const network::ApiRequest& request);

 private:
  ISessionAccounts& accounts_;
  WalletService& service_;
};

}  // namespace rtype::master

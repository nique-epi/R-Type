#pragma once

#include <string>
#include <string_view>
#include "ExchangeReceipt.hpp"
#include "ExchangeRequest.hpp"
#include "LookPurchase.hpp"
#include "MasterTransport.hpp"
#include "OwnedLooks.hpp"
#include "User.hpp"
#include "Wallet.hpp"

namespace rtype::network {

/**
 * @brief What the game client calls to read and spend what the account of a
 * session holds on the master: one function per route of the account, each
 * taking the token of the session, sent as Bearer, and never keeping it.
 *
 * The routes of the account are answered over HTTPS only, so the url names an
 * `https://` master, or a master behind a proxy that ends TLS. Every call
 * waits for the answer, as MasterTransport does, so it is made away from the
 * frame loop, and throws as MasterTransport::send() does;
 * InvalidMasterMessageException for an answer that is not the message of its
 * route. One call at a time.
 */
class MasterAccountClient {
 public:
  /** @throws InvalidMasterUrlException as MasterTransport does. */
  explicit MasterAccountClient(const std::string& url);

  /**
   * @returns The account of the session of @p token, `GET /api/users/me`.
   * @throws MasterRefusedException with status 401 for a token the master
   * does not know or that expired.
   */
  [[nodiscard]] User readOwnUser(const std::string& token);

  /** @returns The balances of the account, `GET /api/users/me/wallet`. */
  [[nodiscard]] Wallet readOwnWallet(const std::string& token);

  /** @returns The looks the account owns, `GET /api/users/me/looks`. */
  [[nodiscard]] OwnedLooks listOwnLooks(const std::string& token);

  /**
   * @returns The look @p item, which the account owns once the master
   * answers, and the balances left, `PUT /api/users/me/looks/{item}`. A look
   * the account already owns costs nothing more.
   * @throws MasterRefusedException with status 404 for a name no look has, 422
   * for a look the shop does not sell, 409 when the price is more than the
   * kopecks.
   */
  [[nodiscard]] LookPurchase buyOwnLook(const std::string& token,
                                        std::string_view item);

  /**
   * @returns What the cashier changed for @p request, and the balances after
   * it, `POST /api/users/me/wallet/exchanges`.
   * @throws MasterRefusedException with status 409 when the account holds
   * less than the amount, or the other balance would pass its largest; 422
   * for an amount of none.
   */
  [[nodiscard]] ExchangeReceipt exchange(const std::string& token,
                                         const ExchangeRequest& request);

 private:
  MasterTransport transport_;
};

}  // namespace rtype::network

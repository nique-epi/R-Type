#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "ExchangeRequest.hpp"
#include "UserReading.hpp"
#include "WalletChangeReading.hpp"
#include "WalletReading.hpp"

namespace rtype::client {

/**
 * @brief The calls to the account of a session on the master, without the
 * frame ever waiting on HTTP: an ask returns its number at once, and its
 * reading shows up on a later frame. A newer ask of a kind replaces the one
 * before it, unserved.
 *
 * Each ask takes the token of the session, which the link keeps in memory
 * only.
 */
class IAccountLink {
 public:
  virtual ~IAccountLink() = default;

  /** @returns The number of this ask for the account of @p token. */
  virtual std::uint64_t askUser(const std::string& token) = 0;

  /** @brief Asks the balances and the looks of the account of @p token;
   * wallet() answers it, with the token it was read with. */
  virtual void askWallet(const std::string& token) = 0;

  /** @returns The number of this purchase of the look @p item by the account
   * of @p token. */
  virtual std::uint64_t askPurchase(const std::string& token,
                                    std::string_view item) = 0;

  /** @returns The number of this exchange of @p request for the account of
   * @p token. */
  virtual std::uint64_t askExchange(
      const std::string& token, const network::ExchangeRequest& request) = 0;

  /** @returns The newest account read, nothing before the first. */
  [[nodiscard]] virtual std::optional<UserReading> user() const = 0;

  /** @returns The newest wallet read, nothing before the first. A purchase
   * or an exchange made gives the balances it answered. */
  [[nodiscard]] virtual std::optional<WalletReading> wallet() const = 0;

  /** @returns The newest answer to a purchase, nothing before the first. */
  [[nodiscard]] virtual std::optional<WalletChangeReading> purchase() const = 0;

  /** @returns The newest answer to an exchange, nothing before the first. */
  [[nodiscard]] virtual std::optional<WalletChangeReading> exchange() const = 0;
};

}  // namespace rtype::client

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include "ExchangeRequest.hpp"
#include "ShipLocker.hpp"
#include "Wallet.hpp"
#include "WalletChangeReading.hpp"

namespace rtype::client {

/**
 * @brief What the account signed in holds on the master, as the screens read
 * and spend it: its kopecks and chips, and the looks it owns. Nothing is kept
 * on the client: every balance is the newest the master answered for the
 * token signed in, and a purchase or an exchange is made by the master, its
 * answer showing up on a later frame.
 *
 * Signed out, it holds nothing and asks nothing: the screens show that a
 * sign-in is needed.
 */
class IWallet {
 public:
  virtual ~IWallet() = default;

  /** @returns The newest balances read for the account signed in; nothing
   * signed out, before the first reading, or when the master did not answer
   * the last one. */
  [[nodiscard]] virtual std::optional<network::Wallet> wallet() const = 0;

  /** @returns The looks the account signed in owns, as wallet() reads them.
   */
  [[nodiscard]] virtual std::optional<ShipLocker> ownedLooks() const = 0;

  /** @brief Asks the master the balances and the looks again; nothing signed
   * out. */
  virtual void askWallet() = 0;

  /** @brief Asks the master to buy @p item, a position in
   * identifiers::SHIP_LOOK_ITEM_NAMES.
   * @returns The number of the purchase, which purchase() answers;
   * NO_ACCOUNT_ASK signed out, which nothing answers. */
  virtual std::uint64_t buy(std::size_t item) = 0;

  /** @returns The answer to the purchase numbered @p ask; nothing until it
   * came. */
  [[nodiscard]] virtual std::optional<WalletChangeReading> purchase(
      std::uint64_t ask) const = 0;

  /** @brief Asks the cashier of the master for @p request.
   * @returns The number of the exchange, which exchanged() answers;
   * NO_ACCOUNT_ASK signed out, which nothing answers. */
  virtual std::uint64_t exchange(const network::ExchangeRequest& request) = 0;

  /** @returns The answer to the exchange numbered @p ask; nothing until it
   * came. */
  [[nodiscard]] virtual std::optional<WalletChangeReading> exchanged(
      std::uint64_t ask) const = 0;
};

}  // namespace rtype::client

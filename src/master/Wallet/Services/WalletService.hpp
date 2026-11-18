#pragma once

#include <SQLiteCpp/Database.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <variant>
#include "ChipMovement.hpp"
#include "ChipMovementResult.hpp"
#include "ExchangeReceipt.hpp"
#include "ExchangeRequest.hpp"
#include "GameCredit.hpp"
#include "GameCreditOutcome.hpp"
#include "IAccountWallet.hpp"
#include "LookPurchaseResult.hpp"
#include "MasterDatabase.hpp"
#include "MovementListing.hpp"
#include "OwnedLooks.hpp"
#include "PageRequest.hpp"
#include "Wallet.hpp"
#include "WalletExchange.hpp"
#include "WalletExchangeRefusal.hpp"

namespace rtype::master {

/**
 * @brief The rules of the wallets of the accounts. A wallet opens at its first
 * read, with the starting balances and the starter looks
 * (network::STARTER_LOCKER_ITEMS). Each change of a balance writes the balance
 * and its line of the journal in one transaction; no balance goes under 0 or
 * past its largest value. No route lets a player add money: kopecks come from
 * games and the cashier, chips from the cashier and the casino.
 */
class WalletService final : public IAccountWallet {
 public:
  /**
   * @param database It must outlive the service.
   * @param startingWallet Gives the balances a wallet opens with, read at
   * each opening, so a setting changed before the start applies.
   */
  WalletService(MasterDatabase& database,
                std::function<network::Wallet()> startingWallet);

  /** @returns The balances of the wallet of @p userId, opened at @p now when
   * it never was. */
  [[nodiscard]] network::Wallet walletOf(std::int64_t userId,
                                         std::chrono::sys_seconds now);

  /** @returns The lines of the journal of @p userId @p page asks for, newest
   * first; the wallet opens at @p now when it never was. */
  [[nodiscard]] network::MovementListing movementsOf(
      std::int64_t userId, const PageRequest& page,
      std::chrono::sys_seconds now);

  /**
   * @brief Changes the amount of @p request into the other currency, one for
   * one, at @p now: two lines of the journal naming one exchange.
   * @returns The exchange and the balances it left; or InvalidAmount for an
   * amount of none or past the largest balance, NotEnough when the balance it
   * leaves holds less, PastTheMaximum when the other balance would pass its
   * largest value.
   */
  [[nodiscard]] std::variant<network::ExchangeReceipt,
                             network::WalletExchangeRefusal>
  exchange(std::int64_t userId, const network::ExchangeRequest& request,
           std::chrono::sys_seconds now);

  /** @returns The exchange @p exchangeId of @p userId, nothing when the
   * account has no exchange of that number. */
  [[nodiscard]] std::optional<network::WalletExchange> exchangeOf(
      std::int64_t userId, std::int64_t exchangeId);

  /** @returns The looks @p userId owns, in the order of
   * identifiers::SHIP_LOOK_ITEM_NAMES; the wallet opens at @p now when it
   * never was. */
  [[nodiscard]] network::OwnedLooks ownedLooksOf(std::int64_t userId,
                                                 std::chrono::sys_seconds now);

  /**
   * @brief Buys the look @p item, a position in
   * identifiers::SHIP_LOOK_ITEM_NAMES, for @p userId at @p now, at its
   * price (network::lookItemPrice()). A look the account owns costs nothing
   * again.
   * @returns What it did, and the balances it left.
   */
  [[nodiscard]] LookPurchaseResult buyLook(std::int64_t userId,
                                           std::size_t item,
                                           std::chrono::sys_seconds now);

  [[nodiscard]] ChipMovementResult applyChips(
      SQLite::Database& connection, const ChipMovement& movement) override;

  GameCreditOutcome creditGame(const GameCredit& credit) override;

 private:
  /** @returns The balances of the wallet of @p userId on @p connection,
   * opened at @p now when it never was; nothing when no account has that id.
   */
  [[nodiscard]] std::optional<network::Wallet> openedWallet(
      SQLite::Database& connection, std::int64_t userId,
      std::chrono::sys_seconds now);

  MasterDatabase& database_;
  std::function<network::Wallet()> startingWallet_;
};

}  // namespace rtype::master

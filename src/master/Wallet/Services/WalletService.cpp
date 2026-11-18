#include "WalletService.hpp"
#include <SQLiteCpp/Database.h>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include "ChipMovement.hpp"
#include "ChipMovementResult.hpp"
#include "ChipRefusal.hpp"
#include "CreditedResultRepository.hpp"
#include "CreditedWallet.hpp"
#include "Currency.hpp"
#include "ExchangeOrder.hpp"
#include "ExchangeReceipt.hpp"
#include "ExchangeRequest.hpp"
#include "GameCredit.hpp"
#include "GameCreditOutcome.hpp"
#include "GameTicketRepository.hpp"
#include "GameTicketText.hpp"
#include "KopecksConstants.hpp"
#include "LookItems.hpp"
#include "LookOrder.hpp"
#include "LookOrigin.hpp"
#include "LookOwnership.hpp"
#include "LookPurchaseOutcome.hpp"
#include "LookPurchaseResult.hpp"
#include "MasterDatabase.hpp"
#include "MovementListing.hpp"
#include "MovementReason.hpp"
#include "MovementRecord.hpp"
#include "OwnedLookRepository.hpp"
#include "OwnedLooks.hpp"
#include "PageRequest.hpp"
#include "ShipLookConstants.hpp"
#include "StarterLookConstants.hpp"
#include "Wallet.hpp"
#include "WalletBalances.hpp"
#include "WalletExchange.hpp"
#include "WalletExchangeRefusal.hpp"
#include "WalletMovementRepository.hpp"
#include "WalletOpening.hpp"
#include "WalletRepository.hpp"
#include "WalletServiceConstants.hpp"

namespace rtype::master {

namespace {

/** @brief Gives the account of @p opening the looks of the starter kit. */
void addStarterLooks(SQLite::Database& connection,
                     const WalletOpening& opening) {
  OwnedLookRepository looks{connection};
  for (std::size_t item = 0; item < identifiers::SHIP_LOOK_ITEM_COUNT; ++item) {
    if (((network::STARTER_LOCKER_ITEMS >> item) & 1U) != 0) {
      looks.insert(LookOwnership{
          .userId = opening.userId,
          .item = std::string{identifiers::SHIP_LOOK_ITEM_NAMES.at(item)},
          .origin = LookOrigin::Starter,
          .acquiredAt = opening.openedAt});
    }
  }
}

/** @brief Writes in the journal the starting balance of @p currency, when
 * the wallet of @p opening starts with some. */
void recordOpening(SQLite::Database& connection, const WalletOpening& opening,
                   network::Currency currency) {
  const std::uint32_t starting = network::balanceIn(opening.starting, currency);
  if (starting == 0) {
    return;
  }
  WalletMovementRepository{connection}.insert(
      MovementRecord{.id = std::nullopt,
                     .userId = opening.userId,
                     .currency = currency,
                     .amount = starting,
                     .reason = network::MovementReason::Opening,
                     .reference = std::string{OPENING_REFERENCE},
                     .madeAt = opening.openedAt});
}

/** @returns @p wallet bounded by the largest balances, as the settings of the
 * starting balances never pass them. */
network::Wallet boundedWallet(const network::Wallet& wallet) {
  return network::Wallet{
      .kopecks = std::min(wallet.kopecks, network::MAXIMUM_KOPECKS),
      .chips = std::min(wallet.chips, network::MAXIMUM_CHIPS)};
}

/** @brief Writes @p order: the two lines of the exchange, which name the
 * first, and the balances it leaves.
 * @returns The exchange and those balances. */
network::ExchangeReceipt writtenExchange(SQLite::Database& connection,
                                         const ExchangeOrder& order) {
  WalletMovementRepository movements{connection};
  const std::int64_t exchangeId = movements.nextId();
  MovementRecord line{.id = exchangeId,
                      .userId = order.userId,
                      .currency = order.request.from,
                      .amount = -std::int64_t{order.request.amount},
                      .reason = network::MovementReason::Exchange,
                      .reference = std::string{EXCHANGE_REFERENCE_PREFIX} +
                                   std::to_string(exchangeId),
                      .madeAt = order.madeAt};
  movements.insert(line);
  line.id = std::nullopt;
  line.currency = network::otherCurrency(order.request.from);
  line.amount = order.request.amount;
  movements.insert(line);
  const network::Wallet after =
      network::exchangedWallet(order.before, order.request);
  WalletRepository{connection}.replace(order.userId, after);
  return network::ExchangeReceipt{
      .exchange = network::WalletExchange{.id = exchangeId,
                                          .from = order.request.from,
                                          .amount = order.request.amount,
                                          .madeAt = order.madeAt},
      .wallet = after};
}

/** @returns Why the shop does not sell the look of @p order to its account,
 * nothing when it does. */
std::optional<LookPurchaseOutcome> lookRefusalOf(SQLite::Database& connection,
                                                 const LookOrder& order) {
  const std::string name{identifiers::SHIP_LOOK_ITEM_NAMES.at(order.item)};
  if (OwnedLookRepository{connection}.owns(order.userId, name)) {
    return LookPurchaseOutcome::AlreadyOwned;
  }
  const std::uint32_t price = network::lookItemPrice(order.item);
  if (price == network::NOT_FOR_SALE) {
    return LookPurchaseOutcome::NotForSale;
  }
  if (!network::canSpend(order.before, network::Currency::Kopecks, price)) {
    return LookPurchaseOutcome::NotEnoughKopecks;
  }
  return std::nullopt;
}

/** @brief Writes @p order: the price taken, its line of the journal and the
 * look owned.
 * @returns The balances it leaves. */
network::Wallet writtenPurchase(SQLite::Database& connection,
                                const LookOrder& order) {
  const std::string name{identifiers::SHIP_LOOK_ITEM_NAMES.at(order.item)};
  const std::uint32_t price = network::lookItemPrice(order.item);
  WalletMovementRepository{connection}.insert(
      MovementRecord{.id = std::nullopt,
                     .userId = order.userId,
                     .currency = network::Currency::Kopecks,
                     .amount = -std::int64_t{price},
                     .reason = network::MovementReason::Purchase,
                     .reference = std::string{LOOK_REFERENCE_PREFIX} + name,
                     .madeAt = order.boughtAt});
  OwnedLookRepository{connection}.insert(
      LookOwnership{.userId = order.userId,
                    .item = name,
                    .origin = LookOrigin::Purchase,
                    .acquiredAt = order.boughtAt});
  const network::Wallet after = network::withBalance(
      order.before, network::Currency::Kopecks, order.before.kopecks - price);
  WalletRepository{connection}.replace(order.userId, after);
  return after;
}

/** @returns The line of the journal @p movement writes once @p moved chips
 * really moved. */
MovementRecord lineOf(const ChipMovement& movement, std::int64_t moved) {
  return MovementRecord{.id = std::nullopt,
                        .userId = movement.userId,
                        .currency = network::Currency::Chips,
                        .amount = moved,
                        .reason = movement.reason,
                        .reference = movement.reference,
                        .madeAt = movement.madeAt};
}

/** @returns Whether @p movement takes more chips than a wallet holding
 * @p before has. */
bool takesTooMany(const ChipMovement& movement, const network::Wallet& before) {
  return movement.amount < 0 && -movement.amount > std::int64_t{before.chips};
}

/** @brief Writes @p movement on a wallet holding @p before: its line of the
 * journal, with what really moved, and the chips it leaves.
 * @returns Those chips. */
std::uint32_t writtenChips(SQLite::Database& connection,
                           const ChipMovement& movement,
                           const network::Wallet& before) {
  const std::uint32_t chips =
      movement.amount >= 0
          ? network::creditedBalance(
                before, network::Currency::Chips,
                static_cast<std::uint64_t>(movement.amount))
          : static_cast<std::uint32_t>(before.chips + movement.amount);
  WalletMovementRepository{connection}.insert(
      lineOf(movement, std::int64_t{chips} - before.chips));
  WalletRepository{connection}.replace(
      movement.userId,
      network::withBalance(before, network::Currency::Chips, chips));
  return chips;
}

/** @brief Writes @p credit on @p wallet: the kopecks, their line of the
 * journal, and the looks won. */
void writtenCredit(SQLite::Database& connection, const GameCredit& credit,
                   const CreditedWallet& wallet) {
  const network::Wallet& before = wallet.before;
  if (credit.kopecks > 0) {
    const std::uint32_t kopecks = network::creditedBalance(
        before, network::Currency::Kopecks, credit.kopecks);
    WalletMovementRepository{connection}.insert(
        MovementRecord{.id = std::nullopt,
                       .userId = wallet.userId,
                       .currency = network::Currency::Kopecks,
                       .amount = std::int64_t{kopecks} - before.kopecks,
                       .reason = network::MovementReason::Game,
                       .reference = credit.reference,
                       .madeAt = credit.creditedAt});
    WalletRepository{connection}.replace(
        wallet.userId,
        network::withBalance(before, network::Currency::Kopecks, kopecks));
  }
  OwnedLookRepository looks{connection};
  for (const std::string& item : credit.wonLooks) {
    if (network::lookItemOfName(item).has_value()) {
      looks.insert(LookOwnership{.userId = wallet.userId,
                                 .item = item,
                                 .origin = LookOrigin::Game,
                                 .acquiredAt = credit.creditedAt});
    }
  }
}

}  // namespace

WalletService::WalletService(MasterDatabase& database,
                             std::function<network::Wallet()> startingWallet)
    : database_(database), startingWallet_(std::move(startingWallet)) {}

network::Wallet WalletService::walletOf(std::int64_t userId,
                                        std::chrono::sys_seconds now) {
  return database_.transact([this, userId, now](SQLite::Database& connection) {
    return openedWallet(connection, userId, now).value_or(network::Wallet{});
  });
}

network::MovementListing WalletService::movementsOf(
    std::int64_t userId, const PageRequest& page,
    std::chrono::sys_seconds now) {
  return database_.transact([this, userId, &page,
                             now](SQLite::Database& connection) {
    static_cast<void>(openedWallet(connection, userId, now));
    return network::MovementListing{
        .movements = WalletMovementRepository{connection}.pageOf(userId, page)};
  });
}

std::variant<network::ExchangeReceipt, network::WalletExchangeRefusal>
WalletService::exchange(std::int64_t userId,
                        const network::ExchangeRequest& request,
                        std::chrono::sys_seconds now) {
  return database_.transact(
      [this, userId, &request, now](SQLite::Database& connection)
          -> std::variant<network::ExchangeReceipt,
                          network::WalletExchangeRefusal> {
        const ExchangeOrder order{
            .userId = userId,
            .before = openedWallet(connection, userId, now)
                          .value_or(network::Wallet{}),
            .request = request,
            .madeAt = now};
        if (const std::optional<network::WalletExchangeRefusal> refusal =
                network::exchangeRefusalOf(order.before, request)) {
          return *refusal;
        }
        return writtenExchange(connection, order);
      });
}

std::optional<network::WalletExchange> WalletService::exchangeOf(
    std::int64_t userId, std::int64_t exchangeId) {
  return database_.run([userId, exchangeId](SQLite::Database& connection) {
    return WalletMovementRepository{connection}.exchangeOf(userId, exchangeId);
  });
}

network::OwnedLooks WalletService::ownedLooksOf(std::int64_t userId,
                                                std::chrono::sys_seconds now) {
  return database_.transact([this, userId, now](SQLite::Database& connection) {
    static_cast<void>(openedWallet(connection, userId, now));
    return network::OwnedLooks{
        .items = network::inCatalogueOrder(
            OwnedLookRepository{connection}.itemsOf(userId))};
  });
}

LookPurchaseResult WalletService::buyLook(std::int64_t userId, std::size_t item,
                                          std::chrono::sys_seconds now) {
  return database_.transact([this, userId, item,
                             now](SQLite::Database& connection) {
    const LookOrder order{
        .userId = userId,
        .item = item,
        .before =
            openedWallet(connection, userId, now).value_or(network::Wallet{}),
        .boughtAt = now};
    if (const std::optional<LookPurchaseOutcome> refusal =
            lookRefusalOf(connection, order)) {
      return LookPurchaseResult{.outcome = *refusal, .wallet = order.before};
    }
    return LookPurchaseResult{.outcome = LookPurchaseOutcome::Bought,
                              .wallet = writtenPurchase(connection, order)};
  });
}

ChipMovementResult WalletService::applyChips(SQLite::Database& connection,
                                             const ChipMovement& movement) {
  const std::optional<network::Wallet> before =
      openedWallet(connection, movement.userId, movement.madeAt);
  if (!before.has_value()) {
    return ChipMovementResult{.refusal = ChipRefusal::UnknownAccount,
                              .chips = 0};
  }
  if (takesTooMany(movement, *before)) {
    return ChipMovementResult{.refusal = ChipRefusal::NotEnoughChips,
                              .chips = before->chips};
  }
  if (movement.amount == 0) {
    return ChipMovementResult{.refusal = std::nullopt, .chips = before->chips};
  }
  return ChipMovementResult{
      .refusal = std::nullopt,
      .chips = writtenChips(connection, movement, *before)};
}

GameCreditOutcome WalletService::creditGame(const GameCredit& credit) {
  return database_.transact([this, &credit](SQLite::Database& connection) {
    const std::optional<std::int64_t> userId =
        GameTicketRepository{connection}.redeem(
            gameTicketFingerprintOf(credit.ticket), credit.creditedAt);
    if (!userId.has_value()) {
      return GameCreditOutcome::UnknownTicket;
    }
    const std::optional<network::Wallet> before =
        openedWallet(connection, *userId, credit.creditedAt);
    if (!before.has_value()) {
      return GameCreditOutcome::UnknownAccount;
    }
    if (!CreditedResultRepository{connection}.insert(credit, *userId)) {
      return GameCreditOutcome::AlreadyCredited;
    }
    writtenCredit(connection, credit,
                  CreditedWallet{.userId = *userId, .before = *before});
    return GameCreditOutcome::Credited;
  });
}

std::optional<network::Wallet> WalletService::openedWallet(
    SQLite::Database& connection, std::int64_t userId,
    std::chrono::sys_seconds now) {
  WalletRepository wallets{connection};
  if (const std::optional<network::Wallet> kept = wallets.balancesOf(userId)) {
    return kept;
  }
  const WalletOpening opening{.userId = userId,
                              .starting = boundedWallet(startingWallet_()),
                              .openedAt = now};
  if (!wallets.insertOpened(opening)) {
    return std::nullopt;
  }
  addStarterLooks(connection, opening);
  recordOpening(connection, opening, network::Currency::Kopecks);
  recordOpening(connection, opening, network::Currency::Chips);
  return opening.starting;
}

}  // namespace rtype::master

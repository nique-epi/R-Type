#include "WalletMovementRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "Currency.hpp"
#include "EpochSeconds.hpp"
#include "MasterMessageText.hpp"
#include "MovementReason.hpp"
#include "MovementRecord.hpp"
#include "PageRequest.hpp"
#include "ServersRepositoryConstants.hpp"
#include "WalletExchange.hpp"
#include "WalletMovement.hpp"
#include "WalletRepositoryConstants.hpp"

namespace rtype::master {

namespace {

/** @returns The line the row @p query stands on holds. A currency or a
 * reason the master never writes reads as the first of the enum. */
network::WalletMovement movementIn(const SQLite::Statement& query) {
  return network::WalletMovement{
      .id = query.getColumn(MOVEMENT_ID_COLUMN).getInt64(),
      .currency = network::currencyOf(
                      query.getColumn(MOVEMENT_CURRENCY_COLUMN).getString())
                      .value_or(network::Currency::Kopecks),
      .amount = query.getColumn(MOVEMENT_AMOUNT_COLUMN).getInt64(),
      .reason = network::movementReasonOf(
                    query.getColumn(MOVEMENT_REASON_COLUMN).getString())
                    .value_or(network::MovementReason::Opening),
      .madeAt = dateOfEpochSeconds(
          query.getColumn(MOVEMENT_MADE_AT_COLUMN).getInt64())};
}

}  // namespace

WalletMovementRepository::WalletMovementRepository(SQLite::Database& connection)
    : connection_(connection) {}

std::int64_t WalletMovementRepository::nextId() {
  SQLite::Statement query{connection_, std::string{NEXT_MOVEMENT_ID_QUERY}};
  query.executeStep();
  return query.getColumn(FIRST_COLUMN).getInt64();
}

std::int64_t WalletMovementRepository::insert(const MovementRecord& record) {
  SQLite::Statement insert{connection_, std::string{INSERT_MOVEMENT_STATEMENT}};
  if (record.id.has_value()) {
    insert.bind(std::string{MOVEMENT_ID_PARAMETER}, *record.id);
  }
  insert.bind(std::string{WALLET_USER_ID_PARAMETER}, record.userId);
  insert.bind(std::string{CURRENCY_PARAMETER},
              std::string{network::currencyText(record.currency)});
  insert.bind(std::string{AMOUNT_PARAMETER}, record.amount);
  insert.bind(std::string{REASON_PARAMETER},
              std::string{network::movementReasonText(record.reason)});
  insert.bind(std::string{REFERENCE_PARAMETER}, record.reference);
  insert.bind(std::string{MADE_AT_PARAMETER}, epochSecondsOf(record.madeAt));
  insert.executeStep();
  return insert.getColumn(FIRST_COLUMN).getInt64();
}

std::vector<network::WalletMovement> WalletMovementRepository::pageOf(
    std::int64_t userId, const PageRequest& page) {
  SQLite::Statement query{connection_, std::string{MOVEMENT_PAGE_QUERY}};
  query.bind(std::string{WALLET_USER_ID_PARAMETER}, userId);
  query.bind(std::string{LIMIT_PARAMETER}, page.limit);
  query.bind(std::string{OFFSET_PARAMETER}, page.offset);
  std::vector<network::WalletMovement> movements;
  while (query.executeStep()) {
    movements.push_back(movementIn(query));
  }
  return movements;
}

std::optional<network::WalletExchange> WalletMovementRepository::exchangeOf(
    std::int64_t userId, std::int64_t exchangeId) {
  SQLite::Statement query{connection_, std::string{EXCHANGE_DEBIT_QUERY}};
  query.bind(std::string{WALLET_USER_ID_PARAMETER}, userId);
  query.bind(std::string{MOVEMENT_ID_PARAMETER}, exchangeId);
  query.bind(std::string{REASON_PARAMETER},
             std::string{network::movementReasonText(
                 network::MovementReason::Exchange)});
  if (!query.executeStep()) {
    return std::nullopt;
  }
  const network::WalletMovement debit = movementIn(query);
  return network::WalletExchange{
      .id = debit.id,
      .from = debit.currency,
      .amount = static_cast<std::uint32_t>(-debit.amount),
      .madeAt = debit.madeAt};
}

bool WalletMovementRepository::holdsReference(const MovementRecord& line) {
  SQLite::Statement query{connection_, std::string{REFERENCE_HELD_QUERY}};
  query.bind(std::string{WALLET_USER_ID_PARAMETER}, line.userId);
  query.bind(std::string{REASON_PARAMETER},
             std::string{network::movementReasonText(line.reason)});
  query.bind(std::string{REFERENCE_PARAMETER}, line.reference);
  query.executeStep();
  return query.getColumn(FIRST_COLUMN).getInt() != 0;
}

}  // namespace rtype::master

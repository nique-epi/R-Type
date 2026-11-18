#include "WalletRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <cstdint>
#include <optional>
#include <string>
#include "EpochSeconds.hpp"
#include "Wallet.hpp"
#include "WalletOpening.hpp"
#include "WalletRepositoryConstants.hpp"

namespace rtype::master {

WalletRepository::WalletRepository(SQLite::Database& connection)
    : connection_(connection) {}

std::optional<network::Wallet> WalletRepository::balancesOf(
    std::int64_t userId) {
  SQLite::Statement query{connection_, std::string{WALLET_BALANCES_QUERY}};
  query.bind(std::string{WALLET_USER_ID_PARAMETER}, userId);
  if (!query.executeStep()) {
    return std::nullopt;
  }
  return network::Wallet{.kopecks = query.getColumn(KOPECKS_COLUMN).getUInt(),
                         .chips = query.getColumn(CHIPS_COLUMN).getUInt()};
}

bool WalletRepository::insertOpened(const WalletOpening& opening) {
  SQLite::Statement insert{connection_,
                           std::string{INSERT_OPENED_WALLET_STATEMENT}};
  insert.bind(std::string{WALLET_USER_ID_PARAMETER}, opening.userId);
  insert.bind(std::string{KOPECKS_PARAMETER}, opening.starting.kopecks);
  insert.bind(std::string{CHIPS_PARAMETER}, opening.starting.chips);
  insert.bind(std::string{OPENED_AT_PARAMETER},
              epochSecondsOf(opening.openedAt));
  return insert.exec() > 0;
}

void WalletRepository::replace(std::int64_t userId,
                               const network::Wallet& wallet) {
  SQLite::Statement update{connection_,
                           std::string{REPLACE_BALANCES_STATEMENT}};
  update.bind(std::string{KOPECKS_PARAMETER}, wallet.kopecks);
  update.bind(std::string{CHIPS_PARAMETER}, wallet.chips);
  update.bind(std::string{WALLET_USER_ID_PARAMETER}, userId);
  update.exec();
}

}  // namespace rtype::master

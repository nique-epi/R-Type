#include "OwnedLookRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "EpochSeconds.hpp"
#include "LookOwnership.hpp"
#include "WalletRepositoryConstants.hpp"

namespace rtype::master {

OwnedLookRepository::OwnedLookRepository(SQLite::Database& connection)
    : connection_(connection) {}

bool OwnedLookRepository::insert(const LookOwnership& ownership) {
  SQLite::Statement insert{connection_,
                           std::string{INSERT_OWNED_LOOK_STATEMENT}};
  insert.bind(std::string{WALLET_USER_ID_PARAMETER}, ownership.userId);
  insert.bind(std::string{ITEM_PARAMETER}, ownership.item);
  insert.bind(std::string{ORIGIN_PARAMETER},
              std::string{LOOK_ORIGIN_TEXTS.at(
                  static_cast<std::size_t>(ownership.origin))});
  insert.bind(std::string{ACQUIRED_AT_PARAMETER},
              epochSecondsOf(ownership.acquiredAt));
  return insert.exec() > 0;
}

std::vector<std::string> OwnedLookRepository::itemsOf(std::int64_t userId) {
  SQLite::Statement query{connection_, std::string{OWNED_ITEMS_QUERY}};
  query.bind(std::string{WALLET_USER_ID_PARAMETER}, userId);
  std::vector<std::string> items;
  while (query.executeStep()) {
    items.push_back(query.getColumn(FIRST_COLUMN).getString());
  }
  return items;
}

bool OwnedLookRepository::owns(std::int64_t userId, const std::string& item) {
  SQLite::Statement query{connection_, std::string{OWNS_LOOK_QUERY}};
  query.bind(std::string{WALLET_USER_ID_PARAMETER}, userId);
  query.bind(std::string{ITEM_PARAMETER}, item);
  query.executeStep();
  return query.getColumn(FIRST_COLUMN).getInt() != 0;
}

}  // namespace rtype::master

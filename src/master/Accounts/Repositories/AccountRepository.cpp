#include "AccountRepository.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <variant>
#include "AccountRefusal.hpp"
#include "AccountRole.hpp"
#include "AccountRow.hpp"
#include "AccountsRepositoryConstants.hpp"
#include "CreationQuota.hpp"
#include "CredentialsRequest.hpp"
#include "EpochSeconds.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageText.hpp"
#include "NewAccount.hpp"
#include "PasswordHash.hpp"
#include "ServersRepositoryConstants.hpp"
#include "StoredAccount.hpp"

namespace rtype::master {

namespace {

std::int64_t creationsOn(SQLite::Database& connection,
                         const std::string& address,
                         std::chrono::sys_seconds since) {
  SQLite::Statement query{connection,
                          std::string{ACCOUNT_CREATIONS_SINCE_QUERY}};
  query.bind(std::string{CREATED_BY_PARAMETER}, address);
  query.bind(std::string{SINCE_PARAMETER}, epochSecondsOf(since));
  query.executeStep();
  return query.getColumn(COUNT_COLUMN).getInt64();
}

/** @returns The id of @p account once added on @p connection, or nothing when
 * another account has its nickname. */
std::optional<std::int64_t> insertedAccountId(SQLite::Database& connection,
                                              const NewAccount& account) {
  SQLite::Statement insert{connection, std::string{INSERT_ACCOUNT_STATEMENT}};
  insert.bind(std::string{ACCOUNT_NICKNAME_PARAMETER}, account.nickname);
  insert.bind(std::string{PASSWORD_HASH_PARAMETER}, account.passwordHash.data(),
              static_cast<int>(account.passwordHash.size()));
  insert.bind(
      std::string{ROLE_PARAMETER},
      std::string{network::accountRoleText(network::AccountRole::Player)});
  insert.bind(std::string{CREATED_BY_PARAMETER}, account.createdByAddress);
  insert.bind(std::string{CREATED_AT_PARAMETER},
              epochSecondsOf(account.createdAt));
  if (!insert.executeStep()) {
    return std::nullopt;
  }
  return insert.getColumn(INSERTED_ACCOUNT_ID_COLUMN).getInt64();
}

/** @returns The hash the row @p query stands on keeps; all zeros, which no
 * password matches, when it is not PASSWORD_HASH_BYTES long. */
network::PasswordHash passwordHashIn(const SQLite::Statement& query) {
  network::PasswordHash hash{};
  const SQLite::Column column = query.getColumn(PASSWORD_HASH_COLUMN);
  if (static_cast<std::size_t>(column.getBytes()) == hash.size()) {
    const std::span<const std::uint8_t> kept{
        static_cast<const std::uint8_t*>(column.getBlob()), hash.size()};
    std::ranges::copy(kept, hash.begin());
  }
  return hash;
}

}  // namespace

AccountRepository::AccountRepository(MasterDatabase& database)
    : database_(database) {}

std::optional<AccountRefusal> AccountRepository::refusalBeforeHashing(
    const CredentialsRequest& request, const CreationQuota& quota) {
  return database_.run(
      [&request,
       &quota](SQLite::Database& connection) -> std::optional<AccountRefusal> {
        SQLite::Statement query{connection,
                                std::string{CREATIONS_AND_NICKNAME_QUERY}};
        query.bind(std::string{CREATED_BY_PARAMETER}, request.address);
        query.bind(std::string{SINCE_PARAMETER}, epochSecondsOf(quota.since));
        query.bind(std::string{ACCOUNT_NICKNAME_PARAMETER},
                   request.credentials.nickname);
        query.executeStep();
        if (query.getColumn(COUNT_COLUMN).getInt64() >= quota.maximum) {
          return AccountRefusal::TooManyCreations;
        }
        if (query.getColumn(NICKNAME_TAKEN_COLUMN).getInt() != 0) {
          return AccountRefusal::NicknameTaken;
        }
        return std::nullopt;
      });
}

std::variant<std::int64_t, AccountRefusal> AccountRepository::insertWithin(
    const NewAccount& account, const CreationQuota& quota) {
  return database_.transact([&account, &quota](SQLite::Database& connection)
                                -> std::variant<std::int64_t, AccountRefusal> {
    if (creationsOn(connection, account.createdByAddress, quota.since) >=
        quota.maximum) {
      return AccountRefusal::TooManyCreations;
    }
    const std::optional<std::int64_t> accountId =
        insertedAccountId(connection, account);
    if (!accountId.has_value()) {
      return AccountRefusal::NicknameTaken;
    }
    return *accountId;
  });
}

std::optional<StoredAccount> AccountRepository::accountNamed(
    const std::string& nickname) {
  return database_.run([&nickname](SQLite::Database& connection)
                           -> std::optional<StoredAccount> {
    SQLite::Statement query{connection, std::string{ACCOUNT_NAMED_QUERY}};
    query.bind(std::string{ACCOUNT_NICKNAME_PARAMETER}, nickname);
    if (!query.executeStep()) {
      return std::nullopt;
    }
    return StoredAccount{.account = accountIn(query),
                         .passwordHash = passwordHashIn(query)};
  });
}

bool AccountRepository::giveRole(const std::string& nickname,
                                 network::AccountRole role) {
  return database_.run([&nickname, role](SQLite::Database& connection) {
    SQLite::Statement update{connection, std::string{UPDATE_ROLE_STATEMENT}};
    update.bind(std::string{ROLE_PARAMETER},
                std::string{network::accountRoleText(role)});
    update.bind(std::string{ACCOUNT_NICKNAME_PARAMETER}, nickname);
    return update.exec() > 0;
  });
}

}  // namespace rtype::master

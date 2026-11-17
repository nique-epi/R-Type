#include "UserRepository.hpp"
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
#include "AccountsRepositoryConstants.hpp"
#include "CreationQuota.hpp"
#include "CredentialsAttempt.hpp"
#include "CredentialsRefusal.hpp"
#include "EpochSeconds.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageText.hpp"
#include "NewUser.hpp"
#include "PasswordHash.hpp"
#include "ServersRepositoryConstants.hpp"
#include "StoredUser.hpp"
#include "UserRole.hpp"
#include "UserRow.hpp"

namespace rtype::master {

namespace {

std::int64_t creationsOn(SQLite::Database& connection,
                         const std::string& address,
                         std::chrono::sys_seconds since) {
  SQLite::Statement query{connection, std::string{USER_CREATIONS_SINCE_QUERY}};
  query.bind(std::string{CREATED_BY_PARAMETER}, address);
  query.bind(std::string{SINCE_PARAMETER}, epochSecondsOf(since));
  query.executeStep();
  return query.getColumn(COUNT_COLUMN).getInt64();
}

/** @returns The id of @p user once added on @p connection, or nothing when
 * another account has its nickname. */
std::optional<std::int64_t> insertedUserId(SQLite::Database& connection,
                                           const NewUser& user) {
  SQLite::Statement insert{connection, std::string{INSERT_USER_STATEMENT}};
  insert.bind(std::string{USER_NICKNAME_PARAMETER}, user.nickname);
  insert.bind(std::string{PASSWORD_HASH_PARAMETER}, user.passwordHash.data(),
              static_cast<int>(user.passwordHash.size()));
  insert.bind(std::string{ROLE_PARAMETER},
              std::string{network::userRoleText(network::UserRole::Player)});
  insert.bind(std::string{CREATED_BY_PARAMETER}, user.createdByAddress);
  insert.bind(std::string{CREATED_AT_PARAMETER},
              epochSecondsOf(user.createdAt));
  if (!insert.executeStep()) {
    return std::nullopt;
  }
  return insert.getColumn(INSERTED_USER_ID_COLUMN).getInt64();
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

UserRepository::UserRepository(MasterDatabase& database)
    : database_(database) {}

std::optional<CredentialsRefusal> UserRepository::refusalBeforeHashing(
    const CredentialsAttempt& request, const CreationQuota& quota) {
  return database_.run([&request, &quota](SQLite::Database& connection)
                           -> std::optional<CredentialsRefusal> {
    SQLite::Statement query{connection,
                            std::string{CREATIONS_AND_NICKNAME_QUERY}};
    query.bind(std::string{CREATED_BY_PARAMETER}, request.address);
    query.bind(std::string{SINCE_PARAMETER}, epochSecondsOf(quota.since));
    query.bind(std::string{USER_NICKNAME_PARAMETER},
               request.credentials.nickname);
    query.executeStep();
    if (query.getColumn(COUNT_COLUMN).getInt64() >= quota.maximum) {
      return CredentialsRefusal::TooManyCreations;
    }
    if (query.getColumn(NICKNAME_TAKEN_COLUMN).getInt() != 0) {
      return CredentialsRefusal::NicknameTaken;
    }
    return std::nullopt;
  });
}

std::variant<std::int64_t, CredentialsRefusal> UserRepository::insertWithin(
    const NewUser& user, const CreationQuota& quota) {
  return database_.transact(
      [&user, &quota](SQLite::Database& connection)
          -> std::variant<std::int64_t, CredentialsRefusal> {
        if (creationsOn(connection, user.createdByAddress, quota.since) >=
            quota.maximum) {
          return CredentialsRefusal::TooManyCreations;
        }
        const std::optional<std::int64_t> userId =
            insertedUserId(connection, user);
        if (!userId.has_value()) {
          return CredentialsRefusal::NicknameTaken;
        }
        return *userId;
      });
}

std::optional<StoredUser> UserRepository::userNamed(
    const std::string& nickname) {
  return database_.run(
      [&nickname](SQLite::Database& connection) -> std::optional<StoredUser> {
        SQLite::Statement query{connection, std::string{USER_NAMED_QUERY}};
        query.bind(std::string{USER_NICKNAME_PARAMETER}, nickname);
        if (!query.executeStep()) {
          return std::nullopt;
        }
        return StoredUser{.user = userIn(query),
                          .passwordHash = passwordHashIn(query)};
      });
}

bool UserRepository::giveRole(const std::string& nickname,
                              network::UserRole role) {
  return database_.run([&nickname, role](SQLite::Database& connection) {
    SQLite::Statement update{connection, std::string{UPDATE_ROLE_STATEMENT}};
    update.bind(std::string{ROLE_PARAMETER},
                std::string{network::userRoleText(role)});
    update.bind(std::string{USER_NICKNAME_PARAMETER}, nickname);
    return update.exec() > 0;
  });
}

}  // namespace rtype::master

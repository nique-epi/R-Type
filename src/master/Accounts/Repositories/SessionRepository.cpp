#include "SessionRepository.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <optional>
#include <string>
#include "AccountsRepositoryConstants.hpp"
#include "ActiveSession.hpp"
#include "EpochSeconds.hpp"
#include "MasterDatabase.hpp"
#include "NewSession.hpp"
#include "ServersRepositoryConstants.hpp"
#include "StatementParts.hpp"
#include "UserRow.hpp"

namespace rtype::master {

namespace {

void insertOn(SQLite::Database& connection, const NewSession& session) {
  SQLite::Statement insert{connection, std::string{INSERT_SESSION_STATEMENT}};
  insert.bind(std::string{TOKEN_FINGERPRINT_PARAMETER},
              session.tokenFingerprint);
  insert.bind(std::string{SESSION_USER_ID_PARAMETER}, session.userId);
  insert.bind(std::string{ADDRESS_PARAMETER}, session.address);
  insert.bind(std::string{CREATED_AT_PARAMETER},
              epochSecondsOf(session.createdAt));
  insert.bind(std::string{EXPIRES_AT_PARAMETER},
              epochSecondsOf(session.expiresAt));
  insert.exec();
}

}  // namespace

SessionRepository::SessionRepository(MasterDatabase& database)
    : database_(database) {}

void SessionRepository::addRemovingExpired(const NewSession& session) {
  database_.transact([&session](SQLite::Database& connection) {
    eraseOlderThan(connection, DELETE_EXPIRED_SESSIONS_STATEMENT,
                   session.createdAt);
    insertOn(connection, session);
  });
}

std::optional<ActiveSession> SessionRepository::ownerOf(
    const std::string& fingerprint, std::chrono::sys_seconds now) {
  return database_.run(
      [&fingerprint,
       now](SQLite::Database& connection) -> std::optional<ActiveSession> {
        SQLite::Statement query{connection, std::string{SESSION_OWNER_QUERY}};
        query.bind(std::string{TOKEN_FINGERPRINT_PARAMETER}, fingerprint);
        query.bind(std::string{NOW_PARAMETER}, epochSecondsOf(now));
        if (!query.executeStep()) {
          return std::nullopt;
        }
        return ActiveSession{
            .user = userIn(query),
            .expiresAt = dateOfEpochSeconds(
                query.getColumn(SESSION_EXPIRES_AT_COLUMN).getInt64())};
      });
}

void SessionRepository::extend(const std::string& fingerprint,
                               std::chrono::sys_seconds expiresAt) {
  database_.run([&fingerprint, expiresAt](SQLite::Database& connection) {
    SQLite::Statement update{connection, std::string{EXTEND_SESSION_STATEMENT}};
    update.bind(std::string{EXPIRES_AT_PARAMETER}, epochSecondsOf(expiresAt));
    update.bind(std::string{TOKEN_FINGERPRINT_PARAMETER}, fingerprint);
    update.exec();
  });
}

void SessionRepository::remove(const std::string& fingerprint) {
  database_.run([&fingerprint](SQLite::Database& connection) {
    SQLite::Statement remove{connection, std::string{DELETE_SESSION_STATEMENT}};
    remove.bind(std::string{TOKEN_FINGERPRINT_PARAMETER}, fingerprint);
    remove.exec();
  });
}

}  // namespace rtype::master

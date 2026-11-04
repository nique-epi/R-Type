#include "ServerRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include "CreationQuota.hpp"
#include "HeartbeatRecord.hpp"
#include "KnownServer.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageText.hpp"
#include "NewServer.hpp"
#include "ServerLimits.hpp"
#include "ServerRecord.hpp"
#include "ServerRegistration.hpp"
#include "ServersRepositoryConstants.hpp"

namespace rtype::master {

namespace {

std::int64_t secondsOf(std::chrono::sys_seconds date) {
  return static_cast<std::int64_t>(date.time_since_epoch().count());
}

std::int64_t creationsSince(SQLite::Database& connection,
                            const std::string& address,
                            std::chrono::sys_seconds since) {
  SQLite::Statement query{connection, std::string{CREATIONS_SINCE_QUERY}};
  query.bind(std::string{CREATED_BY_PARAMETER}, address);
  query.bind(std::string{SINCE_PARAMETER}, secondsOf(since));
  query.executeStep();
  return query.getColumn(CREATION_COUNT_COLUMN).getInt64();
}

std::int64_t insertedServerId(SQLite::Database& connection,
                              const NewServer& server) {
  SQLite::Statement insert{connection, std::string{INSERT_SERVER_STATEMENT}};
  insert.bind(std::string{SERVER_NAME_PARAMETER}, server.name);
  insert.bind(std::string{KEY_FINGERPRINT_PARAMETER}, server.keyFingerprint);
  insert.bind(std::string{CREATED_BY_PARAMETER}, server.createdByAddress);
  insert.bind(std::string{CREATED_AT_PARAMETER}, secondsOf(server.createdAt));
  insert.executeStep();
  return insert.getColumn(SERVER_ID_COLUMN).getInt64();
}

/** @returns The bounds of the row @p query stands on; none while null. */
std::optional<ServerLimits> limitsIn(const SQLite::Statement& query) {
  if (query.getColumn(MAXIMUM_GAMES_COLUMN).isNull()) {
    return std::nullopt;
  }
  return ServerLimits{.maximumGames = static_cast<std::uint16_t>(
                          query.getColumn(MAXIMUM_GAMES_COLUMN).getUInt()),
                      .maximumPlayersPerGame = static_cast<std::uint8_t>(
                          query.getColumn(MAXIMUM_PLAYERS_COLUMN).getUInt())};
}

void bindRegistration(SQLite::Statement& statement,
                      const network::ServerRegistration& registration) {
  statement.bind(std::string{SERVER_NAME_PARAMETER}, registration.name);
  statement.bind(std::string{ADDRESS_PARAMETER},
                 registration.address.value_or(std::string{}));
  statement.bind(std::string{UDP_PORT_PARAMETER}, registration.udpPort);
  statement.bind(std::string{GAME_VERSION_PARAMETER}, registration.gameVersion);
  statement.bind(std::string{PROTOCOL_VERSION_PARAMETER},
                 registration.protocolVersion);
  statement.bind(std::string{MAXIMUM_GAMES_PARAMETER},
                 registration.maximumGames);
  statement.bind(std::string{MAXIMUM_PLAYERS_PARAMETER},
                 registration.maximumPlayersPerGame);
}

}  // namespace

ServerRepository::ServerRepository(MasterDatabase& database)
    : database_(database) {}

std::optional<std::int64_t> ServerRepository::insertWithin(
    const NewServer& server, const CreationQuota& quota) {
  return database_.run(
      [&server,
       &quota](SQLite::Database& connection) -> std::optional<std::int64_t> {
        SQLite::Transaction transaction{connection,
                                        SQLite::TransactionBehavior::IMMEDIATE};
        if (creationsSince(connection, server.createdByAddress, quota.since) >=
            quota.maximum) {
          return std::nullopt;
        }
        const std::int64_t serverId = insertedServerId(connection, server);
        transaction.commit();
        return serverId;
      });
}

std::optional<KnownServer> ServerRepository::serverOfKey(
    const std::string& fingerprint) {
  return database_.run([&fingerprint](SQLite::Database& connection)
                           -> std::optional<KnownServer> {
    SQLite::Statement query{connection, std::string{SERVER_OF_KEY_QUERY}};
    query.bind(std::string{KEY_FINGERPRINT_PARAMETER}, fingerprint);
    if (!query.executeStep()) {
      return std::nullopt;
    }
    return KnownServer{.id = query.getColumn(SERVER_ID_COLUMN).getInt64(),
                       .limits = limitsIn(query)};
  });
}

void ServerRepository::saveRegistration(const ServerRecord& record) {
  database_.run([&record](SQLite::Database& connection) {
    SQLite::Statement update{connection,
                             std::string{UPDATE_REGISTRATION_STATEMENT}};
    bindRegistration(update, record.registration);
    update.bind(std::string{REGISTERED_AT_PARAMETER},
                secondsOf(record.registeredAt));
    update.bind(std::string{SERVER_ID_PARAMETER}, record.serverId);
    update.exec();
  });
}

void ServerRepository::saveLastHeartbeat(const HeartbeatRecord& record) {
  const std::string text = network::heartbeatText(record.heartbeat);
  database_.run([&record, &text](SQLite::Database& connection) {
    SQLite::Statement update{connection,
                             std::string{UPDATE_LAST_HEARTBEAT_STATEMENT}};
    update.bind(std::string{HEARTBEAT_PARAMETER}, text);
    update.bind(std::string{TAKEN_AT_PARAMETER}, secondsOf(record.receivedAt));
    update.bind(std::string{SERVER_ID_PARAMETER}, record.serverId);
    update.exec();
  });
}

}  // namespace rtype::master

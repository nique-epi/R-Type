#include "ServerRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "HeartbeatRecord.hpp"
#include "HostedGame.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageText.hpp"
#include "RegisteredServer.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerLimits.hpp"
#include "ServerMeasures.hpp"
#include "ServerRecord.hpp"
#include "ServerRegistration.hpp"
#include "ServersConstants.hpp"

namespace rtype::master {

namespace {

std::int64_t secondsOf(std::chrono::sys_seconds date) {
  return static_cast<std::int64_t>(date.time_since_epoch().count());
}

void bindRegistration(SQLite::Statement& statement,
                      const ServerRecord& record) {
  const network::ServerRegistration& registration = record.registration;
  statement.bind(std::string{KEY_ID_PARAMETER}, record.keyId);
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

std::optional<std::int64_t> serverIdOfKey(SQLite::Database& connection,
                                          std::int64_t keyId) {
  SQLite::Statement query{connection, std::string{SERVER_OF_KEY_QUERY}};
  query.bind(std::string{KEY_ID_PARAMETER}, keyId);
  if (!query.executeStep()) {
    return std::nullopt;
  }
  return query.getColumn(SERVER_ID_COLUMN).getInt64();
}

std::int64_t insertedServerId(SQLite::Database& connection,
                              const ServerRecord& record) {
  SQLite::Statement insert{connection, std::string{INSERT_SERVER_STATEMENT}};
  bindRegistration(insert, record);
  insert.bind(std::string{REGISTERED_AT_PARAMETER},
              secondsOf(record.registeredAt));
  insert.executeStep();
  return insert.getColumn(SERVER_ID_COLUMN).getInt64();
}

void updateServer(SQLite::Database& connection, const ServerRecord& record) {
  SQLite::Statement update{connection, std::string{UPDATE_SERVER_STATEMENT}};
  bindRegistration(update, record);
  update.exec();
}

/** @returns The server @p record names, inserted or updated. */
RegisteredServer savedServer(SQLite::Database& connection,
                             const ServerRecord& record) {
  const std::optional<std::int64_t> known =
      serverIdOfKey(connection, record.keyId);
  if (!known.has_value()) {
    return RegisteredServer{.id = insertedServerId(connection, record),
                            .created = true};
  }
  updateServer(connection, record);
  return RegisteredServer{.id = *known, .created = false};
}

void bindMeasure(SQLite::Statement& statement, std::string_view parameter,
                 std::optional<double> measure) {
  if (measure.has_value()) {
    statement.bind(std::string{parameter}, *measure);
  } else {
    statement.bind(std::string{parameter});
  }
}

std::int64_t playerCountOf(const network::ServerHeartbeat& heartbeat) {
  std::int64_t count = 0;
  for (const network::HostedGame& game : heartbeat.games) {
    count += static_cast<std::int64_t>(game.players.size());
  }
  return count;
}

void insertSample(SQLite::Database& connection, const HeartbeatRecord& record) {
  const network::ServerMeasures& measures = record.heartbeat.measures;
  SQLite::Statement insert{connection, std::string{INSERT_SAMPLE_STATEMENT}};
  insert.bind(std::string{SERVER_ID_PARAMETER}, record.serverId);
  insert.bind(std::string{TAKEN_AT_PARAMETER}, secondsOf(record.receivedAt));
  insert.bind(std::string{GAME_COUNT_PARAMETER},
              static_cast<std::int64_t>(record.heartbeat.games.size()));
  insert.bind(std::string{PLAYER_COUNT_PARAMETER},
              playerCountOf(record.heartbeat));
  bindMeasure(insert, TICK_P50_PARAMETER, measures.tickP50Milliseconds);
  bindMeasure(insert, TICK_P95_PARAMETER, measures.tickP95Milliseconds);
  bindMeasure(insert, RECEIVED_BYTES_PARAMETER,
              measures.receivedBytesPerSecond);
  bindMeasure(insert, SENT_BYTES_PARAMETER, measures.sentBytesPerSecond);
  bindMeasure(insert, LOSS_PERCENT_PARAMETER, measures.lossPercent);
  bindMeasure(insert, AVERAGE_PING_PARAMETER, measures.averagePingMilliseconds);
  insert.exec();
}

void updateLastHeartbeat(SQLite::Database& connection,
                         const HeartbeatRecord& record) {
  SQLite::Statement update{connection,
                           std::string{UPDATE_LAST_HEARTBEAT_STATEMENT}};
  update.bind(std::string{HEARTBEAT_PARAMETER},
              network::heartbeatText(record.heartbeat));
  update.bind(std::string{TAKEN_AT_PARAMETER}, secondsOf(record.receivedAt));
  update.bind(std::string{SERVER_ID_PARAMETER}, record.serverId);
  update.exec();
}

void eraseOldSamples(SQLite::Database& connection,
                     std::chrono::sys_seconds now) {
  SQLite::Statement erase{connection, std::string{ERASE_OLD_SAMPLES_STATEMENT}};
  erase.bind(std::string{OLDEST_PARAMETER}, secondsOf(now - SAMPLE_RETENTION));
  erase.exec();
}

}  // namespace

ServerRepository::ServerRepository(MasterDatabase& database)
    : database_(database) {}

RegisteredServer ServerRepository::save(const ServerRecord& record) {
  return database_.run([&record](SQLite::Database& connection) {
    SQLite::Transaction transaction{connection,
                                    SQLite::TransactionBehavior::IMMEDIATE};
    const RegisteredServer saved = savedServer(connection, record);
    transaction.commit();
    return saved;
  });
}

std::optional<ServerLimits> ServerRepository::limitsOfKey(std::int64_t keyId) {
  return database_.run(
      [keyId](SQLite::Database& connection) -> std::optional<ServerLimits> {
        SQLite::Statement query{connection, std::string{SERVER_OF_KEY_QUERY}};
        query.bind(std::string{KEY_ID_PARAMETER}, keyId);
        if (!query.executeStep()) {
          return std::nullopt;
        }
        return ServerLimits{
            .id = query.getColumn(SERVER_ID_COLUMN).getInt64(),
            .maximumGames = static_cast<std::uint16_t>(
                query.getColumn(MAXIMUM_GAMES_COLUMN).getUInt()),
            .maximumPlayersPerGame = static_cast<std::uint8_t>(
                query.getColumn(MAXIMUM_PLAYERS_COLUMN).getUInt())};
      });
}

void ServerRepository::recordHeartbeat(const HeartbeatRecord& record) {
  database_.run([&record](SQLite::Database& connection) {
    SQLite::Transaction transaction{connection,
                                    SQLite::TransactionBehavior::IMMEDIATE};
    updateLastHeartbeat(connection, record);
    insertSample(connection, record);
    eraseOldSamples(connection, record.receivedAt);
    transaction.commit();
  });
}

}  // namespace rtype::master

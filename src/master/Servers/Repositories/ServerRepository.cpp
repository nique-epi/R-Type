#include "ServerRepository.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "CreationQuota.hpp"
#include "EpochSeconds.hpp"
#include "HeartbeatRecord.hpp"
#include "KnownServer.hpp"
#include "LastHeartbeat.hpp"
#include "ListedServer.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageText.hpp"
#include "NewServer.hpp"
#include "PageRequest.hpp"
#include "PlayerCount.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerLimits.hpp"
#include "ServerMeasures.hpp"
#include "ServerRecord.hpp"
#include "ServerRegistration.hpp"
#include "ServersRepositoryConstants.hpp"
#include "StatementParts.hpp"
#include "StatusChangeRecord.hpp"
#include "StatusEvidence.hpp"
#include "WatchedServer.hpp"

namespace rtype::master {

namespace {

std::int64_t creationsSince(SQLite::Database& connection,
                            const std::string& address,
                            std::chrono::sys_seconds since) {
  SQLite::Statement query{connection, std::string{CREATIONS_SINCE_QUERY}};
  query.bind(std::string{CREATED_BY_PARAMETER}, address);
  query.bind(std::string{SINCE_PARAMETER}, epochSecondsOf(since));
  query.executeStep();
  return query.getColumn(CREATION_COUNT_COLUMN).getInt64();
}

std::int64_t insertedServerId(SQLite::Database& connection,
                              const NewServer& server) {
  SQLite::Statement insert{connection, std::string{INSERT_SERVER_STATEMENT}};
  insert.bind(std::string{SERVER_NAME_PARAMETER}, server.name);
  insert.bind(std::string{KEY_FINGERPRINT_PARAMETER}, server.keyFingerprint);
  insert.bind(std::string{CREATED_BY_PARAMETER}, server.createdByAddress);
  insert.bind(std::string{CREATED_AT_PARAMETER},
              epochSecondsOf(server.createdAt));
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

WatchedServer watchedServerIn(const SQLite::Statement& query) {
  return WatchedServer{
      .serverId = query.getColumn(WATCHED_ID_COLUMN).getInt64(),
      .evidence =
          StatusEvidence{
              .registeredAt = dateOfEpochSeconds(
                  query.getColumn(WATCHED_REGISTERED_AT_COLUMN).getInt64()),
              .lastHeartbeatAt =
                  optionalDateIn(query.getColumn(WATCHED_HEARTBEAT_AT_COLUMN)),
              .measures =
                  network::ServerMeasures{
                      .tickP50Milliseconds = std::nullopt,
                      .tickP95Milliseconds =
                          measureIn(query.getColumn(WATCHED_TICK_P95_COLUMN)),
                      .receivedBytesPerSecond = std::nullopt,
                      .sentBytesPerSecond = std::nullopt,
                      .lossPercent = measureIn(
                          query.getColumn(WATCHED_LOSS_PERCENT_COLUMN)),
                      .averagePingMilliseconds = std::nullopt},
              .maintenance =
                  query.getColumn(WATCHED_MAINTENANCE_COLUMN).getInt() != 0},
      .status = statusIn(query.getColumn(WATCHED_STATUS_COLUMN))};
}

/** @returns What the registered server of the row @p query stands on sent
 * when it registered. */
network::ServerRegistration registrationIn(const SQLite::Statement& query) {
  return network::ServerRegistration{
      .name = query.getColumn(LISTED_NAME_COLUMN).getString(),
      .address = query.getColumn(LISTED_ADDRESS_COLUMN).getString(),
      .udpPort = static_cast<std::uint16_t>(
          query.getColumn(LISTED_UDP_PORT_COLUMN).getUInt()),
      .gameVersion = query.getColumn(LISTED_GAME_VERSION_COLUMN).getString(),
      .protocolVersion = static_cast<std::uint8_t>(
          query.getColumn(LISTED_PROTOCOL_VERSION_COLUMN).getUInt()),
      .maximumGames = static_cast<std::uint16_t>(
          query.getColumn(LISTED_MAXIMUM_GAMES_COLUMN).getUInt()),
      .maximumPlayersPerGame = static_cast<std::uint8_t>(
          query.getColumn(LISTED_MAXIMUM_PLAYERS_COLUMN).getUInt())};
}

network::ListedServer listedServerIn(const SQLite::Statement& query) {
  return network::ListedServer{
      .id = query.getColumn(LISTED_ID_COLUMN).getInt64(),
      .registration = registrationIn(query),
      .status = statusIn(query.getColumn(LISTED_STATUS_COLUMN)),
      .gameCount = static_cast<std::uint16_t>(
          query.getColumn(LISTED_GAME_COUNT_COLUMN).getUInt()),
      .playerCount = static_cast<std::uint16_t>(
          query.getColumn(LISTED_PLAYER_COUNT_COLUMN).getUInt())};
}

/** @brief Binds what the list and the statuses read of @p heartbeat to
 * @p statement. */
void bindHeartbeatSummary(SQLite::Statement& statement,
                          const network::ServerHeartbeat& heartbeat) {
  statement.bind(std::string{GAME_COUNT_PARAMETER},
                 static_cast<std::int64_t>(heartbeat.games.size()));
  statement.bind(std::string{PLAYER_COUNT_PARAMETER}, playerCountOf(heartbeat));
  bindMeasure(statement, TICK_P95_PARAMETER,
              heartbeat.measures.tickP95Milliseconds);
  bindMeasure(statement, LOSS_PERCENT_PARAMETER,
              heartbeat.measures.lossPercent);
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
                epochSecondsOf(record.registeredAt));
    update.bind(std::string{SERVER_ID_PARAMETER}, record.serverId);
    update.exec();
  });
}

void ServerRepository::saveLastHeartbeat(SQLite::Database& connection,
                                         const HeartbeatRecord& record) {
  SQLite::Statement update{connection,
                           std::string{UPDATE_LAST_HEARTBEAT_STATEMENT}};
  update.bind(std::string{HEARTBEAT_PARAMETER},
              network::heartbeatText(record.heartbeat));
  update.bind(std::string{TAKEN_AT_PARAMETER},
              epochSecondsOf(record.receivedAt));
  bindHeartbeatSummary(update, record.heartbeat);
  update.bind(std::string{SERVER_ID_PARAMETER}, record.serverId);
  update.exec();
}

std::optional<LastHeartbeat> ServerRepository::lastHeartbeatOf(
    std::int64_t serverId) {
  return database_.run(
      [serverId](SQLite::Database& connection) -> std::optional<LastHeartbeat> {
        SQLite::Statement query{connection,
                                std::string{LAST_HEARTBEAT_OF_SERVER_QUERY}};
        query.bind(std::string{SERVER_ID_PARAMETER}, serverId);
        if (!query.executeStep()) {
          return std::nullopt;
        }
        const SQLite::Column text = query.getColumn(LAST_HEARTBEAT_COLUMN);
        if (text.isNull()) {
          return LastHeartbeat{};
        }
        return LastHeartbeat{.heartbeat =
                                 network::heartbeatFromText(text.getString())};
      });
}

std::vector<WatchedServer> ServerRepository::watchedServers(
    SQLite::Database& connection) {
  SQLite::Statement query{connection, std::string{WATCHED_SERVERS_QUERY}};
  return rowsOf(query, watchedServerIn);
}

void ServerRepository::saveStatuses(
    SQLite::Database& connection,
    const std::vector<StatusChangeRecord>& changes) {
  SQLite::Statement update{connection, std::string{UPDATE_STATUS_STATEMENT}};
  for (const StatusChangeRecord& change : changes) {
    update.bind(std::string{STATUS_PARAMETER},
                std::string{network::serverStatusText(change.change.status)});
    update.bind(std::string{SERVER_ID_PARAMETER}, change.serverId);
    update.exec();
    update.reset();
  }
}

bool ServerRepository::setMaintenance(std::int64_t serverId, bool maintenance) {
  return database_.run([serverId, maintenance](SQLite::Database& connection) {
    SQLite::Statement update{connection,
                             std::string{UPDATE_MAINTENANCE_STATEMENT}};
    update.bind(std::string{MAINTENANCE_PARAMETER},
                static_cast<int>(maintenance));
    update.bind(std::string{SERVER_ID_PARAMETER}, serverId);
    return update.exec() > 0;
  });
}

std::vector<network::ListedServer> ServerRepository::registeredServers(
    const PageRequest& page) {
  return database_.run([&page](SQLite::Database& connection) {
    SQLite::Statement query{connection, std::string{REGISTERED_SERVERS_QUERY}};
    bindPage(query, page);
    return rowsOf(query, listedServerIn);
  });
}

bool ServerRepository::hasServer(std::int64_t serverId) {
  return database_.run([serverId](SQLite::Database& connection) {
    SQLite::Statement query{connection, std::string{SERVER_EXISTS_QUERY}};
    query.bind(std::string{SERVER_ID_PARAMETER}, serverId);
    return query.executeStep();
  });
}

}  // namespace rtype::master

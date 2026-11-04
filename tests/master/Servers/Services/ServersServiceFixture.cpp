#include "ServersServiceFixture.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "CreatedServer.hpp"
#include "CreationAttempt.hpp"
#include "KnownServer.hpp"
#include "MasterMessageTextFixtures.hpp"
#include "MasterTestConstants.hpp"
#include "ServerCreation.hpp"
#include "ServerRecord.hpp"
#include "ServerRegistration.hpp"
#include "ServersService.hpp"
#include "ServersServiceTestConstants.hpp"

namespace {

std::string textOfRow(SQLite::Database& connection, std::string_view sql,
                      std::int64_t serverId) {
  SQLite::Statement query{connection, std::string{sql}};
  query.bind(std::string{TESTED_ID_PARAMETER}, serverId);
  return query.executeStep() ? query.getColumn(ONLY_COLUMN).getString()
                             : std::string{};
}

}  // namespace

rtype::master::ServersService& ServersServiceFixture::service() {
  return service_;
}

std::optional<rtype::network::CreatedServer> ServersServiceFixture::creationAt(
    std::string_view address, std::chrono::sys_seconds date) {
  return service_.createServer(rtype::master::CreationAttempt{
      .creation =
          rtype::network::ServerCreation{.name = std::string{TEST_SERVER_NAME}},
      .address = std::string{address},
      .requestedAt = date});
}

rtype::network::CreatedServer ServersServiceFixture::createdServer() {
  const std::string address =
      std::string{CREATOR_ADDRESS_PREFIX} + std::to_string(++creations_);
  return creationAt(address, FIRST_HEARTBEAT_DATE)
      .value_or(rtype::network::CreatedServer{});
}

rtype::master::KnownServer ServersServiceFixture::registeredServer(
    const rtype::network::ServerRegistration& registration) {
  const rtype::network::CreatedServer created = createdServer();
  service_.replaceOwnServer(
      rtype::master::ServerRecord{.serverId = created.id,
                                  .registration = registration,
                                  .registeredAt = FIRST_HEARTBEAT_DATE});
  return service_.serverOfKey(created.key)
      .value_or(rtype::master::KnownServer{});
}

rtype::network::ServerRegistration ServersServiceFixture::smallRegistration() {
  rtype::network::ServerRegistration small =
      rtype::network::testing::parisRegistration();
  small.maximumGames = ONE_GAME;
  small.maximumPlayersPerGame = ONE_PLAYER_PER_GAME;
  return small;
}

std::int64_t ServersServiceFixture::sampleCount() {
  return databaseFile_.database().run([](SQLite::Database& connection) {
    return connection.execAndGet(std::string{SAMPLE_COUNT_QUERY}).getInt64();
  });
}

std::string ServersServiceFixture::nameOfServer(std::int64_t serverId) {
  return databaseFile_.database().run([serverId](SQLite::Database& connection) {
    return textOfRow(connection, SERVER_NAME_QUERY, serverId);
  });
}

std::string ServersServiceFixture::storedFingerprintOf(std::int64_t serverId) {
  return databaseFile_.database().run([serverId](SQLite::Database& connection) {
    return textOfRow(connection, KEY_FINGERPRINT_QUERY, serverId);
  });
}

std::optional<std::string> ServersServiceFixture::lastHeartbeatOf(
    std::int64_t serverId) {
  return databaseFile_.database().run(
      [serverId](SQLite::Database& connection) -> std::optional<std::string> {
        SQLite::Statement query{connection, std::string{LAST_HEARTBEAT_QUERY}};
        query.bind(std::string{TESTED_ID_PARAMETER}, serverId);
        if (!query.executeStep() || query.getColumn(ONLY_COLUMN).isNull()) {
          return std::nullopt;
        }
        return query.getColumn(ONLY_COLUMN).getString();
      });
}

void ServersServiceFixture::restart() { databaseFile_.restart(); }

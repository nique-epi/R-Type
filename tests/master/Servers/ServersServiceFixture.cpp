#include "ServersServiceFixture.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <cstdint>
#include <optional>
#include <string>
#include "MasterMessageTextFixtures.hpp"
#include "ServerRecord.hpp"
#include "ServerRegistration.hpp"
#include "ServersService.hpp"
#include "ServersServiceTestConstants.hpp"

ServersServiceFixture::ServersServiceFixture() {
  database_.open((folder_.path() / SERVERS_DATABASE_FILE).string());
}

rtype::master::ServersService& ServersServiceFixture::service() {
  return service_;
}

std::int64_t ServersServiceFixture::newKeyId() {
  const std::string key =
      keys_.issue(std::string{TEST_KEY_NAME}, FIRST_HEARTBEAT_DATE);
  return keys_.keyIdOf(key).value_or(0);
}

std::int64_t ServersServiceFixture::keyIdOfASmallServer() {
  const std::int64_t keyId = newKeyId();
  rtype::network::ServerRegistration small =
      rtype::network::testing::parisRegistration();
  small.maximumGames = ONE_GAME;
  small.maximumPlayersPerGame = ONE_PLAYER_PER_GAME;
  static_cast<void>(service_.registerServer(
      rtype::master::ServerRecord{.keyId = keyId,
                                  .registration = small,
                                  .registeredAt = FIRST_HEARTBEAT_DATE}));
  return keyId;
}

std::int64_t ServersServiceFixture::sampleCount() {
  return database_.run([](SQLite::Database& connection) {
    return connection.execAndGet(std::string{SAMPLE_COUNT_QUERY}).getInt64();
  });
}

std::string ServersServiceFixture::nameOfServer(std::int64_t serverId) {
  return database_.run([serverId](SQLite::Database& connection) {
    SQLite::Statement query{connection, std::string{SERVER_NAME_QUERY}};
    query.bind(std::string{TESTED_ID_PARAMETER}, serverId);
    return query.executeStep() ? query.getColumn(ONLY_COLUMN).getString()
                               : std::string{};
  });
}

std::optional<std::string> ServersServiceFixture::lastHeartbeatOf(
    std::int64_t serverId) {
  return database_.run(
      [serverId](SQLite::Database& connection) -> std::optional<std::string> {
        SQLite::Statement query{connection, std::string{LAST_HEARTBEAT_QUERY}};
        query.bind(std::string{TESTED_ID_PARAMETER}, serverId);
        if (!query.executeStep() || query.getColumn(ONLY_COLUMN).isNull()) {
          return std::nullopt;
        }
        return query.getColumn(ONLY_COLUMN).getString();
      });
}

void ServersServiceFixture::restart() {
  database_.close();
  database_.open((folder_.path() / SERVERS_DATABASE_FILE).string());
}

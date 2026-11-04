#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <string>
#include "MasterDatabaseFile.hpp"
#include "ServerKeys.hpp"
#include "ServersService.hpp"

/** @brief The servers of the master on a database of their own, with keys. */
class ServersServiceFixture : public ::testing::Test {
 protected:
  [[nodiscard]] rtype::master::ServersService& service();

  /** @returns The id of a key issued for the test. */
  [[nodiscard]] std::int64_t newKeyId();

  /**
   * @returns The id of the key of a server registered as "Paris 1" but for
   * one game of one player at most.
   */
  [[nodiscard]] std::int64_t keyIdOfASmallServer();

  /** @returns How many samples of heartbeats the database holds. */
  [[nodiscard]] std::int64_t sampleCount();

  /** @returns The name the server @p serverId registered with. */
  [[nodiscard]] std::string nameOfServer(std::int64_t serverId);

  /** @returns The last heartbeat of the server @p serverId, or nothing. */
  [[nodiscard]] std::optional<std::string> lastHeartbeatOf(
      std::int64_t serverId);

  /** @brief Closes the database and opens it again, as a restart does. */
  void restart();

 private:
  MasterDatabaseFile databaseFile_;
  rtype::master::ServerKeys keys_{databaseFile_.database()};
  rtype::master::ServersService service_{databaseFile_.database()};
};

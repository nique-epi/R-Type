#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "CreatedServer.hpp"
#include "KnownServer.hpp"
#include "MasterDatabaseFile.hpp"
#include "ServerRegistration.hpp"
#include "ServersService.hpp"

/** @brief The servers of the master on a database of their own. */
class ServersServiceFixture : public ::testing::Test {
 protected:
  [[nodiscard]] rtype::master::ServersService& service();

  /** @returns What a creation of "Paris 1" from @p address at @p date gives. */
  [[nodiscard]] std::optional<rtype::network::CreatedServer> creationAt(
      std::string_view address, std::chrono::sys_seconds date);

  /**
   * @returns A server created as "Paris 1" at FIRST_HEARTBEAT_DATE, each from
   * an address of its own, so no quota refuses it.
   */
  [[nodiscard]] rtype::network::CreatedServer createdServer();

  /** @returns A server created, then registered with @p registration. */
  [[nodiscard]] rtype::master::KnownServer registeredServer(
      const rtype::network::ServerRegistration& registration);

  /** @returns The registration of "Paris 1" for one game of one player. */
  [[nodiscard]] static rtype::network::ServerRegistration smallRegistration();

  /** @returns How many samples of heartbeats the database holds. */
  [[nodiscard]] std::int64_t sampleCount();

  /** @returns The name the server @p serverId has. */
  [[nodiscard]] std::string nameOfServer(std::int64_t serverId);

  /** @returns The fingerprint the server @p serverId keeps of its key. */
  [[nodiscard]] std::string storedFingerprintOf(std::int64_t serverId);

  /** @returns The last heartbeat of the server @p serverId, or nothing. */
  [[nodiscard]] std::optional<std::string> lastHeartbeatOf(
      std::int64_t serverId);

  /** @brief Closes the database and opens it again, as a restart does. */
  void restart();

 private:
  MasterDatabaseFile databaseFile_;
  rtype::master::ServersService service_{databaseFile_.database()};
  std::size_t creations_{0};
};

#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "DatedStatus.hpp"
#include "KnownServer.hpp"
#include "MasterDatabaseFile.hpp"
#include "ServerStatusService.hpp"
#include "ServersService.hpp"

/** @brief The statuses of the servers, and the servers, on a database of
 * their own. */
class ServerStatusServiceFixture : public ::testing::Test {
 protected:
  [[nodiscard]] rtype::master::ServerStatusService& statuses();
  [[nodiscard]] rtype::master::ServersService& servers();

  /** @returns A server created, then registered as "Paris 1" at @p date, each
   * from an address of its own, so no quota refuses it. */
  [[nodiscard]] rtype::master::KnownServer registeredAt(
      std::chrono::sys_seconds date);

  /** @brief Records the heartbeat of "Paris 1", its tick p95 @p tickP95, for
   * @p server, received at @p date. */
  void beat(const rtype::master::KnownServer& server, double tickP95,
            std::chrono::sys_seconds date);

  /** @brief Registers the server @p serverId again at @p date, as it does when
   * it restarts. */
  void registerAgain(std::int64_t serverId, std::chrono::sys_seconds date);

  /** @returns The changes of status of @p serverId, newest first. */
  [[nodiscard]] std::vector<rtype::network::DatedStatus> historyOf(
      std::int64_t serverId);

 private:
  MasterDatabaseFile databaseFile_;
  rtype::master::ServersService servers_{databaseFile_.database()};
  rtype::master::ServerStatusService statuses_{databaseFile_.database()};
  std::size_t creations_{0};
};

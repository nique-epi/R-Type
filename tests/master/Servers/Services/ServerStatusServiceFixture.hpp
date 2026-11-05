#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>
#include "KnownServer.hpp"
#include "MasterDatabaseFile.hpp"
#include "ServerMeasures.hpp"
#include "ServerStatus.hpp"
#include "ServerStatusChange.hpp"
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

  /** @brief Records the heartbeat of "Paris 1" with @p measures for
   * @p server, received at @p date. */
  void beat(const rtype::master::KnownServer& server,
            const rtype::network::ServerMeasures& measures,
            std::chrono::sys_seconds date);

  /** @returns The status the list gives the server @p serverId, nothing when
   * the list leaves it out. */
  [[nodiscard]] std::optional<rtype::network::ServerStatus> listedStatusOf(
      std::int64_t serverId);

  /** @returns The changes of status of @p serverId, newest first. */
  [[nodiscard]] std::vector<rtype::network::ServerStatusChange> historyOf(
      std::int64_t serverId);

  /** @brief Keeps @p text as the last heartbeat of @p serverId. */
  void keepHeartbeatText(std::int64_t serverId, std::string_view text);

 private:
  MasterDatabaseFile databaseFile_;
  rtype::master::ServersService servers_{databaseFile_.database()};
  rtype::master::ServerStatusService statuses_{databaseFile_.database()};
  std::size_t creations_{0};
};

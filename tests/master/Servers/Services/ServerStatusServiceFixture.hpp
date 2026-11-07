#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
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

  /** @returns A server registered as "Paris 1" at @p date, but at
   * OTHER_UDP_PORT: its probes go to another port of the same address. */
  [[nodiscard]] rtype::master::KnownServer registeredElsewhereAt(
      std::chrono::sys_seconds date);

  /** @brief Records the heartbeat of "Paris 1", its tick p95 @p tickP95, for
   * @p server, received at @p date, answering the probe of that second. */
  void beat(const rtype::master::KnownServer& server, double tickP95,
            std::chrono::sys_seconds date);

  /** @brief Records the heartbeat of "Paris 1" for @p server at @p date,
   * without any token, as a server whose UDP port is closed sends it. */
  void beatWithoutAnswer(const rtype::master::KnownServer& server,
                         std::chrono::sys_seconds date);

  /** @brief Registers the server @p serverId again at @p date, as it does when
   * it restarts. */
  void registerAgain(std::int64_t serverId, std::chrono::sys_seconds date);

  /** @brief Runs @p statement on the database of the test. */
  void execute(std::string_view statement);

  /** @returns The changes of status of @p serverId, newest first. */
  [[nodiscard]] std::vector<rtype::network::DatedStatus> historyOf(
      std::int64_t serverId);

 private:
  /** @returns An address no server of the test was created from yet, so no
   * quota refuses a creation. */
  [[nodiscard]] std::string nextCreatorAddress();

  MasterDatabaseFile databaseFile_;
  rtype::master::ServersService servers_{databaseFile_.database()};
  rtype::master::ServerStatusService statuses_{databaseFile_.database()};
  std::size_t creations_{0};
};

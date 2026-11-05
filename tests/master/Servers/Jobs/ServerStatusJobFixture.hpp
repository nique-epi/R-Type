#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include "MasterDatabaseFile.hpp"
#include "ServerStatus.hpp"
#include "ServerStatusJob.hpp"
#include "ServersService.hpp"

/** @brief A database of the master, its servers, and a job on them. */
class ServerStatusJobFixture : public ::testing::Test {
 protected:
  /** @brief Starts the job, @p interval between two refreshes. */
  void startJob(std::chrono::milliseconds interval);

  /** @brief Destroys the job, as the module does when it stops. */
  void stopJob();

  /** @returns The id of a server registered as "Paris 1" that sent a
   * heartbeat, both dated now. */
  [[nodiscard]] std::int64_t serverBeatingNow();

  /** @returns Whether the list gives @p serverId @p status within
   * STATUS_DEADLINE. */
  [[nodiscard]] bool statusSoonIs(std::int64_t serverId,
                                  rtype::network::ServerStatus status);

 private:
  MasterDatabaseFile databaseFile_;
  rtype::master::ServersService servers_{databaseFile_.database()};
  std::optional<rtype::master::ServerStatusJob> job_;
};

#pragma once

#include <gtest/gtest.h>
#include <optional>
#include <string_view>
#include "CreatedServer.hpp"
#include "HeartbeatJob.hpp"
#include "HeartbeatTimes.hpp"
#include "LatestHostedGames.hpp"
#include "RunningMaster.hpp"

namespace rtype::server::testing {

/** @brief A running master that created one server, and a job for it. */
class HeartbeatJobFixture : public ::testing::Test {
 protected:
  HeartbeatJobFixture();

  /** @brief Starts the job of the server, with @p times. */
  void startJob(HeartbeatTimes times);

  /** @brief Publishes one waiting game whose only player is @p pilot. */
  void publishGameOf(std::string_view pilot);

  /** @returns Whether the last heartbeat the master kept names @p pilot within
   * NOTICE_DEADLINE. */
  [[nodiscard]] bool heartbeatSoonNames(std::string_view pilot) const;

  [[nodiscard]] RunningMaster& master();

 private:
  RunningMaster master_;
  network::CreatedServer created_;
  LatestHostedGames games_;
  std::optional<HeartbeatJob> job_;
};

}  // namespace rtype::server::testing

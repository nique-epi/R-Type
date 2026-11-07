#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <string_view>
#include "CreatedServer.hpp"
#include "HeartbeatJob.hpp"
#include "HeartbeatTimes.hpp"
#include "LatestHostedGames.hpp"
#include "LatestValue.hpp"
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

  /** @returns Whether the last heartbeat the master kept holds @p text, a
   * pilot or a member, within NOTICE_DEADLINE. */
  [[nodiscard]] bool heartbeatSoonNames(std::string_view text) const;

  [[nodiscard]] RunningMaster& master();

  /** @brief Publishes @p token as the token of a probe that just came. */
  void publishProbeToken(std::uint64_t token);

  /** @returns Whether the last heartbeat the master keeps lacks @p text
   * within NOTICE_DEADLINE. */
  [[nodiscard]] bool heartbeatSoonLacks(std::string_view text) const;

 private:
  RunningMaster master_;
  network::CreatedServer created_;
  LatestHostedGames games_;
  concurrency::LatestValue<std::uint64_t> probeTokens_;
  std::optional<HeartbeatJob> job_;
};

}  // namespace rtype::server::testing

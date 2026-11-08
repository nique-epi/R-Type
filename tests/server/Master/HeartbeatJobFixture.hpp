#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "CreatedServer.hpp"
#include "GameResult.hpp"
#include "HeartbeatJob.hpp"
#include "HeartbeatTimes.hpp"
#include "LatestHostedGames.hpp"
#include "LatestValue.hpp"
#include "ResultsFileFixtures.hpp"
#include "RunningMaster.hpp"
#include "TemporaryFolder.hpp"

namespace rtype::server::testing {

/** @brief A running master that created one server, a job for it, and the
 * file of results of its games, in a folder of the test. */
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

  /** @brief Saves @p result in the file of results, as a game process does.
   * @returns The id the file drew for it. */
  [[nodiscard]] std::string saveResult(const GameResult& result) const;

  /** @returns Whether the master keeps @p players rows of the game @p gameId
   * within NOTICE_DEADLINE. */
  [[nodiscard]] bool masterSoonKeeps(std::string_view gameId,
                                     int players) const;

  /** @returns What the file of results says the master answered on the game
   * @p gameId, once it says anything or NOTICE_DEADLINE passed. */
  [[nodiscard]] MasterAnswers soonAnswered(std::string_view gameId) const;

  /** @returns What the file of results says the master answered on the game
   * @p gameId, now. */
  [[nodiscard]] MasterAnswers answersOf(std::string_view gameId) const;

 private:
  TemporaryFolder folder_;
  RunningMaster master_;
  network::CreatedServer created_;
  LatestHostedGames games_;
  concurrency::LatestValue<std::uint64_t> probeTokens_;
  std::optional<HeartbeatJob> job_;
};

}  // namespace rtype::server::testing

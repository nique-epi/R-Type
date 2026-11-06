#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "FarewellSender.hpp"
#include "PlayerRoster.hpp"
#include "RecordingDatagramSender.hpp"
#include "RecordingResultsArchive.hpp"
#include "RunningGame.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"

/** @brief The base of the tests of RunningGame and FarewellSender: the game of
 * a process built by hand, a sender that records, and a pause that only writes
 * down how long it was asked to wait. */
class RunningGameFixture : public ::testing::Test {
 protected:
  /** @brief Hosts a game in runningGame(), with a sender that goes nowhere. */
  void hostGame();

  /** @brief Connects the player at @p index to the game of runningGame(). */
  rtype::server::ConnectedPlayer& join(std::size_t index);

  [[nodiscard]] rtype::server::RunningGame& runningGame();

  /** @returns The roster of the game of runningGame(). */
  [[nodiscard]] rtype::server::PlayerRoster& roster();

  [[nodiscard]] RecordingDatagramSender& sender();
  [[nodiscard]] rtype::server::FarewellSender& farewell();

  /** @returns Every wait the farewell asked for, in order. */
  [[nodiscard]] const std::vector<rtype::engine::Duration>& pauses() const;

 private:
  SimulatedClock clock_;
  RecordingResultsArchive results_;
  rtype::server::RunningGame runningGame_;
  RecordingDatagramSender sender_;
  std::vector<rtype::engine::Duration> pauses_;
  rtype::server::FarewellSender farewell_{
      sender_,
      [this](rtype::engine::Duration pause) { pauses_.push_back(pause); }};
};

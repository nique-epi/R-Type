#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "FarewellSender.hpp"
#include "GameInstance.hpp"
#include "Games.hpp"
#include "RecordingDatagramSender.hpp"
#include "TimeConstants.hpp"
#include "TimerScheduler.hpp"

/** @brief The base of the tests of Games and FarewellSender: games of the
 * server built by hand, a sender that records, and a pause that only writes
 * down how long it was asked to wait. */
class GamesFixture : public ::testing::Test {
 protected:
  /** @brief Adds a game to games(), with a sender that goes nowhere. */
  void addGame();

  /** @brief Connects the player at @p index to the first game of games(). */
  rtype::server::ConnectedPlayer& join(std::size_t index);

  /** @brief Connects the player at @p index to @p game. */
  static rtype::server::ConnectedPlayer& joinIn(
      rtype::server::GameInstance& game, std::size_t index);

  /** @brief Connects players to @p game until every seat is taken. */
  static void fill(rtype::server::GameInstance& game);

  [[nodiscard]] rtype::server::Games& games();
  [[nodiscard]] RecordingDatagramSender& sender();
  [[nodiscard]] rtype::server::FarewellSender& farewell();

  /** @returns Every wait the farewell asked for, in order. */
  [[nodiscard]] const std::vector<rtype::engine::Duration>& pauses() const;

 private:
  rtype::engine::TimerScheduler timers_;
  rtype::server::Games games_;
  RecordingDatagramSender sender_;
  std::vector<rtype::engine::Duration> pauses_;
  rtype::server::FarewellSender farewell_{
      sender_,
      [this](rtype::engine::Duration pause) { pauses_.push_back(pause); }};
};

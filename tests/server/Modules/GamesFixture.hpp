#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EventBus.hpp"
#include "FarewellSender.hpp"
#include "Games.hpp"
#include "RecordingDatagramSender.hpp"
#include "TimeConstants.hpp"

/** @brief The base of the tests of Games and FarewellSender: games of the
 * server built by hand, a sender that records, and a pause that only writes
 * down how long it was asked to wait. */
class GamesFixture : public ::testing::Test {
 protected:
  /** @brief Adds a game to games_, with a sender that goes nowhere. */
  void addGame();

  /** @brief Connects the player at @p index to the game at @p gameIndex of
   * games_. */
  rtype::server::ConnectedPlayer& join(std::size_t gameIndex,
                                       std::size_t index);

  rtype::engine::EventBus events_;
  rtype::server::Games games_;
  RecordingDatagramSender sender_;
  std::vector<rtype::engine::Duration> pauses_;
  rtype::server::FarewellSender farewell_{
      sender_,
      [this](rtype::engine::Duration pause) { pauses_.push_back(pause); }};
};

#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include "BotInputSender.hpp"
#include "BotPilotTestConstants.hpp"
#include "RecordingPlayerInputLink.hpp"
#include "RecordingWaitingRoom.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools::testing {

/** @brief A sender drawing from PILOT_SEED, over a waiting room whose start
 * the test sets and a link that writes down each input. */
class BotInputSenderFixture : public ::testing::Test {
 protected:
  /** @brief Starts, or ends, the game of the waiting room. */
  void setGameStarted(bool started);

  /** @returns In how many of @p frames frames of @p frame the sender said an
   * input went out. */
  [[nodiscard]] std::size_t advanceFor(
      std::size_t frames,
      engine::Duration frame = engine::SIMULATION_TICK_DURATION);

  [[nodiscard]] const RecordingPlayerInputLink& link() const;

 private:
  RecordingWaitingRoom room_;
  RecordingPlayerInputLink link_;
  BotInputSender sender_{PILOT_SEED, room_, link_};
};

}  // namespace rtype::tools::testing

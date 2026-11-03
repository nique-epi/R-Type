#include <gtest/gtest.h>
#include <chrono>
#include "ConnectionConstants.hpp"
#include "PlayerInputPacer.hpp"
#include "ServerSessionTestConstants.hpp"
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"

namespace rtype::client {
namespace {

using testing::INTERVALS_OF_A_STALL;
using testing::PROTOCOL_INPUTS_PER_SECOND;

/** @returns A pacer whose first input, due at once, is already sent. */
PlayerInputPacer pacerAfterTheFirstInput() {
  PlayerInputPacer pacer;
  EXPECT_TRUE(pacer.advance(engine::Duration::zero()));
  return pacer;
}

/**
 * Given the rate of the protocol page, 60 inputs per second
 * When the interval between two inputs is read
 * Then it is one sixtieth of a second
 */
TEST(PlayerInputRate, IsThatOfTheProtocolPage) {
  EXPECT_EQ(PLAYER_INPUT_INTERVAL, std::chrono::duration_cast<engine::Duration>(
                                       std::chrono::seconds(1)) /
                                       PROTOCOL_INPUTS_PER_SECOND);
}

/**
 * Given a pacer that never sent an input
 * When a frame that lasted no time at all ends
 * Then an input is due: the game sends at once
 */
TEST(PlayerInputPacer, TheFirstFrameSendsAtOnce) {
  PlayerInputPacer pacer;

  EXPECT_TRUE(pacer.advance(engine::Duration::zero()));
}

/**
 * Given a pacer that just sent an input
 * When a frame one nanosecond shorter than the interval ends
 * Then no input is due yet
 */
TEST(PlayerInputPacer, WaitsOneIntervalAfterAnInput) {
  PlayerInputPacer pacer = pacerAfterTheFirstInput();

  EXPECT_FALSE(pacer.advance(PLAYER_INPUT_INTERVAL - ONE_NANOSECOND));
}

/**
 * Given a pacer that just sent an input
 * When a frame as long as the interval ends
 * Then the next input is due
 */
TEST(PlayerInputPacer, SendsAgainOnceTheIntervalHasPassed) {
  PlayerInputPacer pacer = pacerAfterTheFirstInput();

  EXPECT_TRUE(pacer.advance(PLAYER_INPUT_INTERVAL));
}

/**
 * Given a pacer whose input came one nanosecond after it was due
 * When a frame one nanosecond shorter than the interval ends
 * Then the next input is due: the nanosecond counts toward it
 */
TEST(PlayerInputPacer, CountsTheTimeAFrameLastsBeyondTheInterval) {
  PlayerInputPacer pacer = pacerAfterTheFirstInput();
  EXPECT_TRUE(pacer.advance(PLAYER_INPUT_INTERVAL + ONE_NANOSECOND));

  EXPECT_TRUE(pacer.advance(PLAYER_INPUT_INTERVAL - ONE_NANOSECOND));
}

/**
 * Given a pacer that just sent an input
 * When a frame three intervals long ends, then a frame of no time
 * Then only the first of them sends an input: a stall never sends a burst
 */
TEST(PlayerInputPacer, NeverSendsTwoInputsForOneStall) {
  PlayerInputPacer pacer = pacerAfterTheFirstInput();
  EXPECT_TRUE(pacer.advance(PLAYER_INPUT_INTERVAL * INTERVALS_OF_A_STALL));

  EXPECT_FALSE(pacer.advance(engine::Duration::zero()));
}

}  // namespace
}  // namespace rtype::client

#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <vector>
#include "ActionSet.hpp"
#include "BotInputSenderFixture.hpp"
#include "BotInputSenderTestConstants.hpp"
#include "BotPilot.hpp"
#include "BotPilotFixtures.hpp"
#include "BotPilotTestConstants.hpp"

using rtype::client::ActionSet;
using rtype::tools::BotPilot;
using rtype::tools::testing::BotInputSenderFixture;
using rtype::tools::testing::FRAMES_IN_A_SECOND;
using rtype::tools::testing::HALF_A_TICK;
using rtype::tools::testing::HALF_TICKS_IN_A_SECOND;
using rtype::tools::testing::heldOver;
using rtype::tools::testing::INPUTS_IN_A_SECOND;
using rtype::tools::testing::PILOT_SEED;

/**
 * Given a sender whose game has not started
 * When a second of frames passes
 * Then no input goes out
 */
TEST_F(BotInputSenderFixture, NothingGoesOutBeforeTheGameStarts) {
  static_cast<void>(advanceFor(FRAMES_IN_A_SECOND));

  EXPECT_TRUE(link().sentActions().empty());
}

/**
 * Given a sender whose game started
 * When a second of frames passes
 * Then the inputs of a second of a client go out
 */
TEST_F(BotInputSenderFixture, SendsAtThePaceOfAClientOnceTheGameStarted) {
  setGameStarted(true);

  static_cast<void>(advanceFor(FRAMES_IN_A_SECOND));

  EXPECT_EQ(link().sentActions().size(), INPUTS_IN_A_SECOND);
}

/**
 * Given a sender that played a second of frames twice as short as a tick
 * When another second of such frames passes
 * Then the inputs of a second of a client go out in it, not one per frame
 */
TEST_F(BotInputSenderFixture, KeepsThePaceOfAClientWithShorterFrames) {
  setGameStarted(true);
  static_cast<void>(advanceFor(HALF_TICKS_IN_A_SECOND, HALF_A_TICK));
  const std::size_t sentBefore = link().sentActions().size();

  static_cast<void>(advanceFor(HALF_TICKS_IN_A_SECOND, HALF_A_TICK));

  EXPECT_EQ(link().sentActions().size() - sentBefore, INPUTS_IN_A_SECOND);
}

/**
 * Given a sender whose game started
 * When a second of frames passes
 * Then each input holds what a pilot of the same seed holds in its frame
 */
TEST_F(BotInputSenderFixture, EachInputHoldsWhatThePilotHolds) {
  BotPilot pilot{PILOT_SEED};
  const std::vector<ActionSet> held = heldOver(pilot, FRAMES_IN_A_SECOND);
  setGameStarted(true);

  static_cast<void>(advanceFor(FRAMES_IN_A_SECOND));

  EXPECT_TRUE(
      std::ranges::equal(link().sentActions(), held, {}, {}, &ActionSet::bits));
}

/**
 * Given a sender that played for a second, then whose game ended
 * When another second of frames passes
 * Then no more input goes out
 */
TEST_F(BotInputSenderFixture, NothingGoesOutOnceTheGameEnded) {
  setGameStarted(true);
  static_cast<void>(advanceFor(FRAMES_IN_A_SECOND));
  setGameStarted(false);

  static_cast<void>(advanceFor(FRAMES_IN_A_SECOND));

  EXPECT_EQ(link().sentActions().size(), INPUTS_IN_A_SECOND);
}

/**
 * Given a sender whose game started
 * When a second of frames passes
 * Then it says an input went out exactly as often as one did
 */
TEST_F(BotInputSenderFixture, SaysWhenAnInputWentOut) {
  setGameStarted(true);

  const std::size_t said = advanceFor(FRAMES_IN_A_SECOND);

  EXPECT_EQ(said, link().sentActions().size());
}

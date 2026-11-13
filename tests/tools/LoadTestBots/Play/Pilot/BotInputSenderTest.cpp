#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "ActionSet.hpp"
#include "BotInputSender.hpp"
#include "BotInputSenderFixture.hpp"
#include "BotInputSenderTestConstants.hpp"
#include "BotPilot.hpp"
#include "BotPilotFixtures.hpp"
#include "BotPilotTestConstants.hpp"
#include "BotSight.hpp"
#include "FixedEntityTypeTable.hpp"
#include "RecordingWaitingRoom.hpp"
#include "RefusingPlayerInputLink.hpp"
#include "TimeConstants.hpp"

using rtype::client::ActionSet;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::tools::BotInputSender;
using rtype::tools::BotPilot;
using rtype::tools::BotSight;
using rtype::tools::testing::ACKNOWLEDGED_INPUT;
using rtype::tools::testing::BOT_PLAYER_NUMBER;
using rtype::tools::testing::BotInputSenderFixture;
using rtype::tools::testing::FixedEntityTypeTable;
using rtype::tools::testing::FRAMES_IN_A_SECOND;
using rtype::tools::testing::HALF_A_TICK;
using rtype::tools::testing::HALF_TICKS_IN_A_SECOND;
using rtype::tools::testing::heldOver;
using rtype::tools::testing::INPUTS_IN_A_SECOND;
using rtype::tools::testing::OTHER_PLAYER_NUMBER;
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
 * Given a sender whose game started, over a link that sends nothing
 * When its first frame passes, in which the pace has an input due
 * Then it says no input went out
 */
TEST(BotInputSender, SaysNothingWentOutWhenTheLinkSendsNothing) {
  RecordingWaitingRoom room;
  room.setGameStarted(true);
  RefusingPlayerInputLink link;
  BotInputSender sender{std::make_unique<BotPilot>(PILOT_SEED), room, link};
  const FixedEntityTypeTable types;
  const BotSight sight{types};

  const bool said = sender.advance(SIMULATION_TICK_DURATION, sight);

  EXPECT_FALSE(said);
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

/**
 * Given a sender
 * When a world state lists the ship of its player, the server having applied
 * input ACKNOWLEDGED_INPUT
 * Then the link confirms the inputs up to that one
 */
TEST_F(BotInputSenderFixture, AWorldStateConfirmsTheInputsTheServerApplied) {
  confirmFrom(stateAcknowledgingFor(BOT_PLAYER_NUMBER), BOT_PLAYER_NUMBER);

  EXPECT_EQ(link().confirmations(),
            std::vector<std::uint32_t>{ACKNOWLEDGED_INPUT});
}

/**
 * Given a sender
 * When a world state lists the ship of another player only
 * Then nothing is confirmed
 */
TEST_F(BotInputSenderFixture, AWorldStateOfAnotherPlayerConfirmsNothing) {
  confirmFrom(stateAcknowledgingFor(OTHER_PLAYER_NUMBER), BOT_PLAYER_NUMBER);

  EXPECT_TRUE(link().confirmations().empty());
}

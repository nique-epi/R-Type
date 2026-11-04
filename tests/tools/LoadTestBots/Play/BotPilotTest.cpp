#include <gtest/gtest.h>
#include <algorithm>
#include <bit>
#include <cstddef>
#include <vector>
#include "ActionSet.hpp"
#include "BotPilot.hpp"
#include "BotPilotFixtures.hpp"
#include "BotPilotTestConstants.hpp"
#include "PlayerAction.hpp"
#include "TimeConstants.hpp"

using rtype::client::ActionSet;
using rtype::client::PlayerAction;
using rtype::engine::Duration;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::tools::BotPilot;
using rtype::tools::testing::A_FIFTH_OF_A_SECOND;
using rtype::tools::testing::EVERY_BOT_ACTION;
using rtype::tools::testing::FRAMES_IN_A_MINUTE;
using rtype::tools::testing::heldOver;
using rtype::tools::testing::OTHER_PILOT_SEED;
using rtype::tools::testing::PILOT_SEED;

/**
 * Given two pilots built from the same seed
 * When both are advanced by the same frames for a minute
 * Then they hold the same action at every frame
 */
TEST(BotPilot, TwoPilotsOfOneSeedHoldTheSameActions) {
  BotPilot first{PILOT_SEED};
  BotPilot second{PILOT_SEED};

  const std::vector<ActionSet> firstHeld = heldOver(first, FRAMES_IN_A_MINUTE);
  const std::vector<ActionSet> secondHeld =
      heldOver(second, FRAMES_IN_A_MINUTE);

  ASSERT_EQ(firstHeld.size(), secondHeld.size());
  for (std::size_t frame = 0; frame < firstHeld.size(); ++frame) {
    EXPECT_EQ(firstHeld[frame].bits(), secondHeld[frame].bits());
  }
}

/**
 * Given two pilots built from two seeds, as two bots of a squad are
 * When both are advanced by the same frames for a minute
 * Then they hold different actions at some frame
 */
TEST(BotPilot, TwoPilotsOfTwoSeedsHoldDifferentActions) {
  BotPilot first{PILOT_SEED};
  BotPilot second{OTHER_PILOT_SEED};

  const std::vector<ActionSet> firstHeld = heldOver(first, FRAMES_IN_A_MINUTE);
  const std::vector<ActionSet> secondHeld =
      heldOver(second, FRAMES_IN_A_MINUTE);

  EXPECT_FALSE(std::ranges::equal(firstHeld, secondHeld, {}, &ActionSet::bits,
                                  &ActionSet::bits));
}

/**
 * Given a pilot
 * When it is advanced for a minute
 * Then it holds exactly one action at every frame
 */
TEST(BotPilot, HoldsOneActionAtEveryFrame) {
  BotPilot pilot{PILOT_SEED};

  const std::vector<ActionSet> held = heldOver(pilot, FRAMES_IN_A_MINUTE);

  for (const ActionSet actions : held) {
    EXPECT_EQ(std::popcount(actions.bits()), 1);
  }
}

/**
 * Given a pilot
 * When it is advanced for a minute
 * Then it held each direction and shoot at some frame
 */
TEST(BotPilot, HoldsEveryActionWithinAMinute) {
  BotPilot pilot{PILOT_SEED};

  const std::vector<ActionSet> held = heldOver(pilot, FRAMES_IN_A_MINUTE);

  for (const PlayerAction action : EVERY_BOT_ACTION) {
    EXPECT_TRUE(std::ranges::any_of(
        held, [action](ActionSet actions) { return actions.has(action); }));
  }
}

/**
 * Given a pilot
 * When it is advanced for a minute
 * Then each action it changes to is held a fifth of a second at least
 */
TEST(BotPilot, KeepsAnActionAFifthOfASecondAtLeast) {
  BotPilot pilot{PILOT_SEED};

  const std::vector<ActionSet> held = heldOver(pilot, FRAMES_IN_A_MINUTE);

  Duration sinceChange{};
  for (std::size_t frame = 1; frame < held.size(); ++frame) {
    sinceChange += SIMULATION_TICK_DURATION;
    if (held[frame].bits() != held[frame - 1].bits()) {
      EXPECT_GE(sinceChange, A_FIFTH_OF_A_SECOND);
      sinceChange = Duration::zero();
    }
  }
}

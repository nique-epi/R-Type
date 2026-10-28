#include <gtest/gtest.h>
#include "ConnectedPlayer.hpp"
#include "InputKeeper.hpp"
#include "PlayerRoster.hpp"
#include "SessionsFixtures.hpp"
#include "SessionsTestConstants.hpp"

namespace rtype::server {

using testing::addPlayer;
using testing::inputNumbered;
using testing::NEWER_INPUT_NUMBER;
using testing::OLDER_INPUT_NUMBER;
using testing::originOf;
using testing::OTHER_ACTIONS;
using testing::SOME_ACTIONS;
using testing::SOME_INPUT_NUMBER;

/**
 * Given a player who sent no input yet
 * When its first input arrives
 * Then it is the newest input of the player
 */
TEST(InputKeeper, KeepsTheFirstInput) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  InputKeeper keeper(roster);

  keeper.keepIfNewer(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));

  EXPECT_EQ(player.newestInput, inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));
}

/**
 * Given a player whose newest input is numbered 100
 * When an input numbered 101 arrives
 * Then it replaces the newest input
 */
TEST(InputKeeper, ReplacesTheNewestInputWithAHigherNumber) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  InputKeeper keeper(roster);
  keeper.keepIfNewer(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));

  keeper.keepIfNewer(originOf(0),
                     inputNumbered(NEWER_INPUT_NUMBER, OTHER_ACTIONS));

  EXPECT_EQ(player.newestInput,
            inputNumbered(NEWER_INPUT_NUMBER, OTHER_ACTIONS));
}

/**
 * Given a player whose newest input is numbered 100
 * When an input numbered 99 arrives
 * Then the newest input is unchanged
 */
TEST(InputKeeper, IgnoresAnInputWithALowerNumber) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  InputKeeper keeper(roster);
  keeper.keepIfNewer(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));

  keeper.keepIfNewer(originOf(0),
                     inputNumbered(OLDER_INPUT_NUMBER, OTHER_ACTIONS));

  EXPECT_EQ(player.newestInput, inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));
}

/**
 * Given a player whose newest input is numbered 100
 * When another input numbered 100 arrives
 * Then the newest input is unchanged
 */
TEST(InputKeeper, IgnoresAnInputNumberedLikeTheNewest) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  InputKeeper keeper(roster);
  keeper.keepIfNewer(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));

  keeper.keepIfNewer(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, OTHER_ACTIONS));

  EXPECT_EQ(player.newestInput, inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));
}

/**
 * Given a roster without the sender
 * When an input arrives from that sender
 * Then no player keeps it
 */
TEST(InputKeeper, IgnoresAnInputFromAnAddressNoPlayerSendsFrom) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  InputKeeper keeper(roster);

  keeper.keepIfNewer(originOf(1),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));

  EXPECT_FALSE(player.newestInput.has_value());
}

}  // namespace rtype::server

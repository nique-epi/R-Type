#include <gtest/gtest.h>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "InputKeeper.hpp"
#include "PlayerRoster.hpp"
#include "SessionsFixtures.hpp"
#include "SessionsTestConstants.hpp"

namespace rtype::server {

using testing::addPlayer;
using testing::ignoreInput;
using testing::inputNumbered;
using testing::NEWER_INPUT_NUMBER;
using testing::OLDER_INPUT_NUMBER;
using testing::originOf;
using testing::OTHER_ACTIONS;
using testing::recorderOf;
using testing::ShownInput;
using testing::SOME_ACTIONS;
using testing::SOME_INPUT_NUMBER;

/**
 * Given a player who sent no input yet
 * When its first input arrives
 * Then its number is the newest input number of the player
 */
TEST(InputKeeper, KeepsTheFirstInput) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  InputKeeper keeper(roster, ignoreInput);

  keeper.keepIfNewer(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));

  EXPECT_EQ(player.newestInputNumber, SOME_INPUT_NUMBER);
}

/**
 * Given a player whose newest input is numbered 100
 * When an input numbered 101 arrives
 * Then its number replaces the newest input number
 */
TEST(InputKeeper, ReplacesTheNewestInputWithAHigherNumber) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  InputKeeper keeper(roster, ignoreInput);
  keeper.keepIfNewer(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));

  keeper.keepIfNewer(originOf(0),
                     inputNumbered(NEWER_INPUT_NUMBER, OTHER_ACTIONS));

  EXPECT_EQ(player.newestInputNumber, NEWER_INPUT_NUMBER);
}

/**
 * Given a player whose newest input is numbered 100
 * When an input numbered 99 arrives
 * Then the newest input number is unchanged
 */
TEST(InputKeeper, IgnoresAnInputWithALowerNumber) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  InputKeeper keeper(roster, ignoreInput);
  keeper.keepIfNewer(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));

  keeper.keepIfNewer(originOf(0),
                     inputNumbered(OLDER_INPUT_NUMBER, OTHER_ACTIONS));

  EXPECT_EQ(player.newestInputNumber, SOME_INPUT_NUMBER);
}

/**
 * Given a player whose newest input is numbered 100
 * When another input numbered 100 arrives
 * Then the newest input number is unchanged
 */
TEST(InputKeeper, IgnoresAnInputNumberedLikeTheNewest) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  InputKeeper keeper(roster, ignoreInput);
  keeper.keepIfNewer(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));

  keeper.keepIfNewer(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, OTHER_ACTIONS));

  EXPECT_EQ(player.newestInputNumber, SOME_INPUT_NUMBER);
}

/**
 * Given a roster without the sender
 * When an input arrives from that sender
 * Then no player keeps its number, and the game is not shown it
 */
TEST(InputKeeper, IgnoresAnInputFromAnAddressNoPlayerSendsFrom) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  std::vector<ShownInput> shown;
  InputKeeper keeper(roster, recorderOf(shown));

  keeper.keepIfNewer(originOf(1),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));

  EXPECT_FALSE(player.newestInputNumber.has_value());
  EXPECT_TRUE(shown.empty());
}

/**
 * Given a player whose newest input is numbered 100
 * When an input numbered 101 arrives
 * Then the game is shown that input while the number 100 is still kept
 */
TEST(InputKeeper, ShowsTheGameANewerInputBeforeKeepingIt) {
  PlayerRoster roster;
  addPlayer(roster);
  std::vector<ShownInput> shown;
  InputKeeper keeper(roster, recorderOf(shown));
  keeper.keepIfNewer(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));

  keeper.keepIfNewer(originOf(0),
                     inputNumbered(NEWER_INPUT_NUMBER, OTHER_ACTIONS));

  ASSERT_EQ(shown.size(), 2U);
  EXPECT_EQ(shown.back().input,
            inputNumbered(NEWER_INPUT_NUMBER, OTHER_ACTIONS));
  EXPECT_EQ(shown.back().numberKeptBefore, SOME_INPUT_NUMBER);
}

/**
 * Given a player whose newest input is numbered 100
 * When an input numbered 99 arrives
 * Then the game is not shown it: only the input numbered 100 was
 */
TEST(InputKeeper, ShowsTheGameNoOlderInput) {
  PlayerRoster roster;
  addPlayer(roster);
  std::vector<ShownInput> shown;
  InputKeeper keeper(roster, recorderOf(shown));
  keeper.keepIfNewer(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));

  keeper.keepIfNewer(originOf(0),
                     inputNumbered(OLDER_INPUT_NUMBER, OTHER_ACTIONS));

  ASSERT_EQ(shown.size(), 1U);
  EXPECT_EQ(shown.front().input.inputNumber, SOME_INPUT_NUMBER);
}

}  // namespace rtype::server

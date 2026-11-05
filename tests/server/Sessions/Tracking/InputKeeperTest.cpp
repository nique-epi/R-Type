#include <gtest/gtest.h>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "InputKeeper.hpp"
#include "PlayerRoster.hpp"
#include "SessionsFixtures.hpp"
#include "SessionsTestConstants.hpp"

namespace rtype::server {

using testing::addPlayer;
using testing::FOURTH_ACTIONS;
using testing::ignoreInput;
using testing::inputNumbered;
using testing::inputRepeating;
using testing::NEWER_INPUT_NUMBER;
using testing::OLDER_INPUT_NUMBER;
using testing::originOf;
using testing::OTHER_ACTIONS;
using testing::recorderOf;
using testing::SECOND_NEWER_INPUT_NUMBER;
using testing::ShownInput;
using testing::SOME_ACTIONS;
using testing::SOME_INPUT_NUMBER;
using testing::THIRD_ACTIONS;
using testing::THIRD_NEWER_INPUT_NUMBER;

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

/**
 * Given a player whose newest input is numbered 100, and a datagram numbered
 * 101 and 102 lost on the way
 * When the input numbered 103 arrives repeating the two before it
 * Then the game is shown 101, 102 and 103 in that order with their own actions
 */
TEST(InputKeeper, ShowsTheGameTheInputsALostDatagramHeld) {
  PlayerRoster roster;
  addPlayer(roster);
  std::vector<ShownInput> shown;
  InputKeeper keeper(roster, recorderOf(shown));
  keeper.keepNewerOf(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));

  keeper.keepNewerOf(originOf(0),
                     inputRepeating(THIRD_NEWER_INPUT_NUMBER, FOURTH_ACTIONS,
                                    {THIRD_ACTIONS, OTHER_ACTIONS}));

  ASSERT_EQ(shown.size(), 4U);
  EXPECT_EQ(shown[1].input, inputNumbered(NEWER_INPUT_NUMBER, OTHER_ACTIONS));
  EXPECT_EQ(shown[2].input,
            inputNumbered(SECOND_NEWER_INPUT_NUMBER, THIRD_ACTIONS));
  EXPECT_EQ(shown[3].input,
            inputNumbered(THIRD_NEWER_INPUT_NUMBER, FOURTH_ACTIONS));
}

/**
 * Given inputs numbered 100 to 103 sent one by one, each repeating the ones
 * before it and none lost
 * When they all arrive
 * Then the game is shown each of them once, in order
 */
TEST(InputKeeper, ShowsTheGameEachRepeatedInputOnlyOnce) {
  PlayerRoster roster;
  addPlayer(roster);
  std::vector<ShownInput> shown;
  InputKeeper keeper(roster, recorderOf(shown));

  keeper.keepNewerOf(originOf(0),
                     inputNumbered(SOME_INPUT_NUMBER, SOME_ACTIONS));
  keeper.keepNewerOf(
      originOf(0),
      inputRepeating(NEWER_INPUT_NUMBER, OTHER_ACTIONS, {SOME_ACTIONS}));
  keeper.keepNewerOf(originOf(0),
                     inputRepeating(SECOND_NEWER_INPUT_NUMBER, THIRD_ACTIONS,
                                    {OTHER_ACTIONS, SOME_ACTIONS}));
  keeper.keepNewerOf(
      originOf(0),
      inputRepeating(THIRD_NEWER_INPUT_NUMBER, FOURTH_ACTIONS,
                     {THIRD_ACTIONS, OTHER_ACTIONS, SOME_ACTIONS}));

  ASSERT_EQ(shown.size(), 4U);
  EXPECT_EQ(shown[0].input.inputNumber, SOME_INPUT_NUMBER);
  EXPECT_EQ(shown[1].input.inputNumber, NEWER_INPUT_NUMBER);
  EXPECT_EQ(shown[2].input.inputNumber, SECOND_NEWER_INPUT_NUMBER);
  EXPECT_EQ(shown[3].input.inputNumber, THIRD_NEWER_INPUT_NUMBER);
}

/**
 * Given a player whose newest input is numbered 103
 * When an input numbered 102 arrives late, repeating 101 and 100
 * Then the game is shown nothing more and the newest number stays 103
 */
TEST(InputKeeper, ShowsTheGameNothingOfAMessageWhoseInputsItAlreadyHas) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  std::vector<ShownInput> shown;
  InputKeeper keeper(roster, recorderOf(shown));
  keeper.keepNewerOf(originOf(0),
                     inputNumbered(THIRD_NEWER_INPUT_NUMBER, SOME_ACTIONS));

  keeper.keepNewerOf(originOf(0),
                     inputRepeating(SECOND_NEWER_INPUT_NUMBER, OTHER_ACTIONS,
                                    {THIRD_ACTIONS, SOME_ACTIONS}));

  EXPECT_EQ(shown.size(), 1U);
  EXPECT_EQ(player.newestInputNumber, THIRD_NEWER_INPUT_NUMBER);
}

/**
 * Given a roster without the sender
 * When a message repeating earlier inputs arrives from that sender
 * Then the game is shown nothing
 */
TEST(InputKeeper, ShowsTheGameNothingOfARepeatingMessageFromAStranger) {
  PlayerRoster roster;
  addPlayer(roster);
  std::vector<ShownInput> shown;
  InputKeeper keeper(roster, recorderOf(shown));

  keeper.keepNewerOf(
      originOf(1),
      inputRepeating(NEWER_INPUT_NUMBER, OTHER_ACTIONS, {SOME_ACTIONS}));

  EXPECT_TRUE(shown.empty());
}

}  // namespace rtype::server

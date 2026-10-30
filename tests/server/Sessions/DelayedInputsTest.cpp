#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DelayedInputs.hpp"
#include "DelayedInputsTestConstants.hpp"
#include "PlayerRoster.hpp"
#include "ServerException.hpp"
#include "SessionsFixtures.hpp"

namespace rtype::server {

using testing::addPlayer;
using testing::FIRST_HELD_ACTIONS;
using testing::FIRST_HELD_INPUT_NUMBER;
using testing::inputNumbered;
using testing::NO_DELAY_TICKS;
using testing::recorderOf;
using testing::SECOND_HELD_ACTIONS;
using testing::SECOND_HELD_INPUT_NUMBER;
using testing::SECOND_PLAYER_INDEX;
using testing::ShownInput;
using testing::SOME_DELAY_TICKS;
using testing::TOO_MANY_DELAY_TICKS;

/**
 * Given an input pushed during the first tick, held for three ticks
 * When the ticks release their inputs
 * Then the input comes out in the fourth tick and not before
 */
TEST(DelayedInputs, ReleasesAnInputOnceItsDelayIsOver) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  std::vector<ShownInput> released;
  DelayedInputs delayed(SOME_DELAY_TICKS);
  delayed.push(player.playerNumber,
               inputNumbered(FIRST_HELD_INPUT_NUMBER, FIRST_HELD_ACTIONS));
  const auto sink = recorderOf(released);

  for (std::size_t tick = 0; tick < SOME_DELAY_TICKS; ++tick) {
    delayed.release(roster, sink);
    EXPECT_TRUE(released.empty()) << "released early in tick " << tick;
  }
  delayed.release(roster, sink);

  ASSERT_EQ(released.size(), 1U);
  EXPECT_EQ(released.front().input.actions, FIRST_HELD_ACTIONS);
}

/**
 * Given an input pushed during a tick, held for no tick
 * When that tick releases its inputs
 * Then the input comes out in that same tick
 */
TEST(DelayedInputs, ReleasesAnInputInTheTickThatPushedItWithoutDelay) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  std::vector<ShownInput> released;
  DelayedInputs delayed(NO_DELAY_TICKS);
  delayed.push(player.playerNumber,
               inputNumbered(FIRST_HELD_INPUT_NUMBER, FIRST_HELD_ACTIONS));

  delayed.release(roster, recorderOf(released));

  EXPECT_EQ(released.size(), 1U);
}

/**
 * Given two players whose inputs were pushed in ticks one apart
 * When the ticks release their inputs
 * Then each input comes out exactly the delay after its own tick, in the order
 * pushed
 */
TEST(DelayedInputs, HoldsEveryPlayerForTheSameNumberOfTicks) {
  PlayerRoster roster;
  const ConnectedPlayer& first = addPlayer(roster);
  const ConnectedPlayer& second = addPlayer(roster, SECOND_PLAYER_INDEX);
  std::vector<ShownInput> released;
  DelayedInputs delayed(SOME_DELAY_TICKS);
  const auto sink = recorderOf(released);
  delayed.push(first.playerNumber,
               inputNumbered(FIRST_HELD_INPUT_NUMBER, FIRST_HELD_ACTIONS));
  delayed.release(roster, sink);
  delayed.push(second.playerNumber,
               inputNumbered(SECOND_HELD_INPUT_NUMBER, SECOND_HELD_ACTIONS));

  for (std::size_t tick = 0; tick < SOME_DELAY_TICKS; ++tick) {
    delayed.release(roster, sink);
  }
  const std::size_t releasedAtTheFirstDelay = released.size();
  delayed.release(roster, sink);

  EXPECT_EQ(releasedAtTheFirstDelay, 1U);
  ASSERT_EQ(released.size(), 2U);
  EXPECT_EQ(released.at(0).input.actions, FIRST_HELD_ACTIONS);
  EXPECT_EQ(released.at(1).input.actions, SECOND_HELD_ACTIONS);
}

/**
 * Given an input held for a player
 * When the inputs of that player are cleared
 * Then it never comes out, and the input of another player still does
 */
TEST(DelayedInputs, ClearDropsTheInputsOfThatPlayerOnly) {
  PlayerRoster roster;
  const ConnectedPlayer& first = addPlayer(roster);
  const ConnectedPlayer& second = addPlayer(roster, SECOND_PLAYER_INDEX);
  std::vector<ShownInput> released;
  DelayedInputs delayed(NO_DELAY_TICKS);
  delayed.push(first.playerNumber,
               inputNumbered(FIRST_HELD_INPUT_NUMBER, FIRST_HELD_ACTIONS));
  delayed.push(second.playerNumber,
               inputNumbered(SECOND_HELD_INPUT_NUMBER, SECOND_HELD_ACTIONS));

  delayed.clear(first.playerNumber);
  delayed.release(roster, recorderOf(released));

  ASSERT_EQ(released.size(), 1U);
  EXPECT_EQ(released.front().input.actions, SECOND_HELD_ACTIONS);
}

/**
 * Given an input held for a player who then left the roster
 * When the input comes due
 * Then it is dropped
 */
TEST(DelayedInputs, DropsAnInputWhosePlayerLeft) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  std::vector<ShownInput> released;
  DelayedInputs delayed(NO_DELAY_TICKS);
  delayed.push(player.playerNumber,
               inputNumbered(FIRST_HELD_INPUT_NUMBER, FIRST_HELD_ACTIONS));
  static_cast<void>(roster.remove(player.endpoint));

  delayed.release(roster, recorderOf(released));

  EXPECT_TRUE(released.empty());
}

/**
 * Given a delay over the most ticks the server holds an input
 * When the inputs are built
 * Then the delay is refused
 */
TEST(DelayedInputs, RefusesADelayOverTheHighest) {
  EXPECT_THROW(DelayedInputs{TOO_MANY_DELAY_TICKS}, InvalidInputDelayException);
}

}  // namespace rtype::server

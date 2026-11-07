#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DelayedInputs.hpp"
#include "DelayedInputsTestConstants.hpp"
#include "PlayerRoster.hpp"
#include "ServerException.hpp"
#include "Sessions/SessionsFixtures.hpp"

namespace rtype::server {

using testing::addPlayer;
using testing::FIRST_HELD_ACTIONS;
using testing::FIRST_HELD_INPUT_NUMBER;
using testing::inputNumbered;
using testing::INPUTS_READ_TOGETHER;
using testing::NO_DELAY_TICKS;
using testing::recorderOf;
using testing::RELEASED_BY_THE_FIRST_CATCH_UP;
using testing::SECOND_HELD_ACTIONS;
using testing::SECOND_HELD_INPUT_NUMBER;
using testing::SECOND_PLAYER_INDEX;
using testing::ShownInput;
using testing::SOME_DELAY_TICKS;
using testing::TICKS_UP_TO_THE_FIRST_CATCH_UP;
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

/**
 * Given two inputs of one player read in the same tick, held for three ticks
 * When the ticks release their inputs
 * Then the first comes out in the fourth tick and the second in the fifth: one
 * input of a player per tick, as the client predicts them
 */
TEST(DelayedInputs, AppliesTwoInputsReadTogetherInTwoTicks) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  std::vector<ShownInput> released;
  DelayedInputs delayed(SOME_DELAY_TICKS);
  const auto sink = recorderOf(released);
  delayed.push(player.playerNumber,
               inputNumbered(FIRST_HELD_INPUT_NUMBER, FIRST_HELD_ACTIONS));
  delayed.push(player.playerNumber,
               inputNumbered(SECOND_HELD_INPUT_NUMBER, SECOND_HELD_ACTIONS));
  for (std::size_t tick = 0; tick < SOME_DELAY_TICKS; ++tick) {
    delayed.release(roster, sink);
  }

  delayed.release(roster, sink);
  const std::size_t releasedInTheFourthTick = released.size();
  delayed.release(roster, sink);

  EXPECT_EQ(releasedInTheFourthTick, 1U);
  ASSERT_EQ(released.size(), 2U);
  EXPECT_EQ(released.at(0).input.actions, FIRST_HELD_ACTIONS);
  EXPECT_EQ(released.at(1).input.actions, SECOND_HELD_ACTIONS);
}

/**
 * Given one input of each of two players read in the same tick, with no delay
 * When that tick releases its inputs
 * Then both come out: each player has its own input of the tick
 */
TEST(DelayedInputs, AppliesOneInputOfEachPlayerInTheSameTick) {
  PlayerRoster roster;
  const ConnectedPlayer& first = addPlayer(roster);
  const ConnectedPlayer& second = addPlayer(roster, SECOND_PLAYER_INDEX);
  std::vector<ShownInput> released;
  DelayedInputs delayed(NO_DELAY_TICKS);
  delayed.push(first.playerNumber,
               inputNumbered(FIRST_HELD_INPUT_NUMBER, FIRST_HELD_ACTIONS));
  delayed.push(second.playerNumber,
               inputNumbered(SECOND_HELD_INPUT_NUMBER, SECOND_HELD_ACTIONS));

  delayed.release(roster, recorderOf(released));

  EXPECT_EQ(released.size(), 2U);
}

/**
 * Given ten inputs of one player read in the same tick, with no delay
 * When eight ticks release their inputs
 * Then nine came out: one per tick while the oldest waited at most six ticks
 * past its delay, then two in the tick it waited seven
 */
TEST(DelayedInputs, CatchesUpOnceAnInputWaitedPastTheMargin) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster);
  std::vector<ShownInput> released;
  DelayedInputs delayed(NO_DELAY_TICKS);
  const auto sink = recorderOf(released);
  for (std::size_t input = 1; input <= INPUTS_READ_TOGETHER; ++input) {
    delayed.push(
        player.playerNumber,
        inputNumbered(static_cast<std::uint32_t>(input), FIRST_HELD_ACTIONS));
  }

  for (std::size_t tick = 0; tick < TICKS_UP_TO_THE_FIRST_CATCH_UP; ++tick) {
    delayed.release(roster, sink);
  }

  EXPECT_EQ(released.size(), RELEASED_BY_THE_FIRST_CATCH_UP);
}

}  // namespace rtype::server

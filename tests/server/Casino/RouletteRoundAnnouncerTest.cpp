#include <gtest/gtest.h>
#include <vector>
#include "CasinoConstants.hpp"
#include "RouletteRound.hpp"
#include "RouletteRoundAnnouncerFixture.hpp"
#include "RouletteRoundAnnouncerTestConstants.hpp"

namespace rtype::server::testing {

using network::RoulettePhase;
using network::RouletteRound;

/**
 * Given two players of the room, the first at the table
 * When the first state of the table is announced
 * Then the first gets it at once, and the one in the room nothing
 */
TEST_F(RouletteRoundAnnouncerFixture, TellsThePlayersAtTheTableAtOnce) {
  join(SEATED_PLAYER);
  join(PLAYER_IN_THE_ROOM);
  const RouletteRound round = roundListing(SEATED_PLAYER);

  announce(round, 1);

  EXPECT_EQ(roundsTo(SEATED_PLAYER), std::vector<RouletteRound>{round});
  EXPECT_TRUE(roundsTo(PLAYER_IN_THE_ROOM).empty());
}

/**
 * Given a player of the room and a table nobody sits at
 * When the state of the table is announced for a whole interval
 * Then nothing is sent
 */
TEST_F(RouletteRoundAnnouncerFixture, SendsNothingForAnEmptyTable) {
  join(PLAYER_IN_THE_ROOM);
  const RouletteRound round{.round = 0,
                            .phase = RoulettePhase::Betting,
                            .ticksLeft = SOME_TICKS_LEFT,
                            .winningNumber = 0,
                            .seats = {}};

  announce(round, TICKS_PER_ROULETTE_ROUND);

  EXPECT_TRUE(roundsTo(PLAYER_IN_THE_ROOM).empty());
}

/**
 * Given a state announced
 * When only its clock moves, for one tick short of the interval
 * Then nothing more is sent: a client counts the clock down itself
 */
TEST_F(RouletteRoundAnnouncerFixture, WaitsTheIntervalWhileOnlyTheClockMoves) {
  join(SEATED_PLAYER);
  RouletteRound round = roundListing(SEATED_PLAYER);
  announce(round, 1);
  round.ticksLeft = ONE_TICK_LATER;

  announce(round, TICKS_PER_ROULETTE_ROUND - 1);

  EXPECT_EQ(roundsTo(SEATED_PLAYER).size(), 1U);
}

/**
 * Given a state announced
 * When the whole interval goes by with nothing but the clock moving
 * Then the state is sent again, with its clock
 */
TEST_F(RouletteRoundAnnouncerFixture, RepeatsTheStateEveryInterval) {
  join(SEATED_PLAYER);
  RouletteRound round = roundListing(SEATED_PLAYER);
  announce(round, 1);
  round.ticksLeft = ONE_TICK_LATER;

  announce(round, TICKS_PER_ROULETTE_ROUND);

  ASSERT_EQ(roundsTo(SEATED_PLAYER).size(), 2U);
  EXPECT_EQ(roundsTo(SEATED_PLAYER).back().ticksLeft, ONE_TICK_LATER);
}

/**
 * Given a state announced
 * When the next tick brings another phase
 * Then it is sent at once
 */
TEST_F(RouletteRoundAnnouncerFixture, SendsAChangeAtOnce) {
  join(SEATED_PLAYER);
  RouletteRound round = roundListing(SEATED_PLAYER);
  announce(round, 1);
  round.round = SECOND_ROUND;

  announce(round, 1);

  ASSERT_EQ(roundsTo(SEATED_PLAYER).size(), 2U);
  EXPECT_EQ(roundsTo(SEATED_PLAYER).back().round, SECOND_ROUND);
}

/**
 * Given a state announced, then forgotten as the table closes
 * When the same state comes again at the next tick
 * Then it is sent at once
 */
TEST_F(RouletteRoundAnnouncerFixture, SendsAtOnceOnceItForgot) {
  join(SEATED_PLAYER);
  const RouletteRound round = roundListing(SEATED_PLAYER);
  announce(round, 1);
  announcer().forget();

  announce(round, 1);

  EXPECT_EQ(roundsTo(SEATED_PLAYER).size(), 2U);
}

}  // namespace rtype::server::testing

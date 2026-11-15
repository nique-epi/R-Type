#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <vector>
#include "CasinoConstants.hpp"
#include "ConnectedPlayer.hpp"
#include "ProtocolConstants.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"
#include "RouletteTableFixture.hpp"
#include "RouletteTableTestConstants.hpp"

namespace rtype::server::testing {

using network::MAXIMUM_ROUND_STAKE;
using network::RouletteBet;
using network::RouletteBetKind;
using network::RoulettePhase;
using network::RouletteSeat;

/**
 * Given a player of the room who is ready
 * When it sits at the table
 * Then it is at the casino, no longer ready, and the round lists it
 */
TEST_F(RouletteTableFixture, SeatsAPlayerWhoIsThenNotReady) {
  join(FIRST_TABLE_SEAT).ready = true;

  sit(FIRST_TABLE_SEAT);

  EXPECT_TRUE(player(FIRST_TABLE_SEAT).atCasino);
  EXPECT_FALSE(player(FIRST_TABLE_SEAT).ready);
  EXPECT_TRUE(listedSeat(FIRST_TABLE_SEAT).has_value());
}

/**
 * Given a seated player with a bet, while the bets are open
 * When it goes back to the room
 * Then it is no longer at the casino and the round no longer lists it
 */
TEST_F(RouletteTableFixture, ForgetsAPlayerWhoLeavesDuringTheBets) {
  join(FIRST_TABLE_SEAT);
  sit(FIRST_TABLE_SEAT);
  bet(FIRST_TABLE_SEAT,
      {RouletteBet{.kind = RouletteBetKind::Red,
                   .firstNumber = 0,
                   .amount = HUNDRED_CHIPS}},
      false);

  standUp(FIRST_TABLE_SEAT);

  EXPECT_FALSE(player(FIRST_TABLE_SEAT).atCasino);
  EXPECT_FALSE(listedSeat(FIRST_TABLE_SEAT).has_value());
}

/**
 * Given a table where nobody sits
 * When more ticks than the bets last go by
 * Then the bets stay open with a full clock, and nothing is drawn
 */
TEST_F(RouletteTableFixture, KeepsTheClockOfAnEmptyTableFull) {
  join(FIRST_TABLE_SEAT);

  runTicks(ROULETTE_BETTING_TICKS + A_FEW_TICKS);

  EXPECT_EQ(table().round().phase, RoulettePhase::Betting);
  EXPECT_EQ(table().round().ticksLeft, ROULETTE_BETTING_TICKS);
  EXPECT_EQ(draw().drawCount(), 0U);
}

/**
 * Given a seated player who does not ask to spin
 * When the bets have run one tick short of their time, then one more
 * Then the bets are still open, then the wheel turns on the scripted number
 */
TEST_F(RouletteTableFixture, SpinsWhenTheClockRunsOut) {
  join(FIRST_TABLE_SEAT);
  sit(FIRST_TABLE_SEAT);

  runTicks(ROULETTE_BETTING_TICKS - 1);
  ASSERT_EQ(table().round().phase, RoulettePhase::Betting);
  runTicks(1);

  EXPECT_EQ(table().round().phase, RoulettePhase::Spinning);
  EXPECT_EQ(table().round().winningNumber, SCRIPTED_NUMBER);
  EXPECT_EQ(draw().drawCount(), 1U);
}

/**
 * Given two seated players
 * When the first asks to spin, a tick goes by, then the second asks too
 * Then the bets stay open after the first vote, and the wheel turns at the
 * tick after the second, well before the clock ends
 */
TEST_F(RouletteTableFixture, SpinsOnceEverySeatedPlayerAsked) {
  join(FIRST_TABLE_SEAT);
  join(SECOND_TABLE_SEAT);
  sit(FIRST_TABLE_SEAT);
  sit(SECOND_TABLE_SEAT);

  bet(FIRST_TABLE_SEAT, {}, true);
  runTicks(1);
  ASSERT_EQ(table().round().phase, RoulettePhase::Betting);
  bet(SECOND_TABLE_SEAT, {}, true);
  runTicks(1);

  EXPECT_EQ(table().round().phase, RoulettePhase::Spinning);
}

/**
 * Given 100 chips straight on the number the script draws
 * When the wheel turns
 * Then the round lists the stake and a payout of 36 times it
 */
TEST_F(RouletteTableFixture, PaysAStraightOnTheNumberDrawn) {
  join(FIRST_TABLE_SEAT);
  sit(FIRST_TABLE_SEAT);
  bet(FIRST_TABLE_SEAT,
      {RouletteBet{.kind = RouletteBetKind::Straight,
                   .firstNumber = SCRIPTED_NUMBER,
                   .amount = HUNDRED_CHIPS}},
      true);

  runTicks(1);

  EXPECT_EQ(listedSeat(FIRST_TABLE_SEAT),
            (RouletteSeat{.playerNumber = player(FIRST_TABLE_SEAT).playerNumber,
                          .stake = HUNDRED_CHIPS,
                          .payout = STRAIGHT_RETURN_OF_A_HUNDRED}));
}

/**
 * Given 100 chips straight on a number next to the one drawn
 * When the wheel turns
 * Then the round lists the stake and no payout
 */
TEST_F(RouletteTableFixture, PaysNothingOnAnotherNumber) {
  join(FIRST_TABLE_SEAT);
  sit(FIRST_TABLE_SEAT);
  bet(FIRST_TABLE_SEAT,
      {RouletteBet{.kind = RouletteBetKind::Straight,
                   .firstNumber = NEIGHBOUR_NUMBER,
                   .amount = HUNDRED_CHIPS}},
      true);

  runTicks(1);

  ASSERT_TRUE(listedSeat(FIRST_TABLE_SEAT).has_value());
  EXPECT_EQ(listedSeat(FIRST_TABLE_SEAT)->payout, 0U);
}

/**
 * Given a seated player
 * When it lays two bets that add up to the cap of a round exactly
 * Then both are taken
 */
TEST_F(RouletteTableFixture, TakesBetsUpToTheCap) {
  join(FIRST_TABLE_SEAT);
  sit(FIRST_TABLE_SEAT);

  bet(FIRST_TABLE_SEAT,
      {RouletteBet{.kind = RouletteBetKind::Red,
                   .firstNumber = 0,
                   .amount = HALF_THE_CAP},
       RouletteBet{.kind = RouletteBetKind::Black,
                   .firstNumber = 0,
                   .amount = HALF_THE_CAP}},
      false);

  ASSERT_TRUE(listedSeat(FIRST_TABLE_SEAT).has_value());
  EXPECT_EQ(listedSeat(FIRST_TABLE_SEAT)->stake, MAXIMUM_ROUND_STAKE);
}

/**
 * Given a seated player who holds 100 chips on the cloth
 * When it sends bets one chip past the cap of a round
 * Then they are refused whole, and its 100 chips stay
 */
TEST_F(RouletteTableFixture, RefusesBetsPastTheCapWhole) {
  join(FIRST_TABLE_SEAT);
  sit(FIRST_TABLE_SEAT);
  bet(FIRST_TABLE_SEAT,
      {RouletteBet{.kind = RouletteBetKind::Red,
                   .firstNumber = 0,
                   .amount = HUNDRED_CHIPS}},
      false);

  bet(FIRST_TABLE_SEAT,
      {RouletteBet{.kind = RouletteBetKind::Red,
                   .firstNumber = 0,
                   .amount = HALF_THE_CAP},
       RouletteBet{.kind = RouletteBetKind::Black,
                   .firstNumber = 0,
                   .amount = ONE_CHIP_MORE}},
      false);

  ASSERT_TRUE(listedSeat(FIRST_TABLE_SEAT).has_value());
  EXPECT_EQ(listedSeat(FIRST_TABLE_SEAT)->stake, HUNDRED_CHIPS);
}

/**
 * Given a seated player
 * When it sends a straight bet and a split that crosses the end of a street
 * Then both are refused
 */
TEST_F(RouletteTableFixture, RefusesBetsWithOneOffTheCloth) {
  join(FIRST_TABLE_SEAT);
  sit(FIRST_TABLE_SEAT);

  bet(FIRST_TABLE_SEAT,
      {RouletteBet{.kind = RouletteBetKind::Straight,
                   .firstNumber = SCRIPTED_NUMBER,
                   .amount = HUNDRED_CHIPS},
       RouletteBet{.kind = RouletteBetKind::Split,
                   .firstNumber = NEIGHBOUR_NUMBER,
                   .amount = HUNDRED_CHIPS}},
      false);

  ASSERT_TRUE(listedSeat(FIRST_TABLE_SEAT).has_value());
  EXPECT_EQ(listedSeat(FIRST_TABLE_SEAT)->stake, 0U);
}

/**
 * Given a seated player
 * When it sends bets for the round after the one in progress
 * Then they are ignored
 */
TEST_F(RouletteTableFixture, IgnoresBetsOfAnotherRound) {
  join(FIRST_TABLE_SEAT);
  sit(FIRST_TABLE_SEAT);

  table().onBets(
      player(FIRST_TABLE_SEAT),
      network::RouletteBets{
          .round = static_cast<std::uint16_t>(table().round().round + 1),
          .spinVote = false,
          .bets = {RouletteBet{.kind = RouletteBetKind::Red,
                               .firstNumber = 0,
                               .amount = HUNDRED_CHIPS}}});

  ASSERT_TRUE(listedSeat(FIRST_TABLE_SEAT).has_value());
  EXPECT_EQ(listedSeat(FIRST_TABLE_SEAT)->stake, 0U);
}

/**
 * Given a player in the room, not at the table
 * When it sends bets
 * Then they are ignored, and the round lists nobody
 */
TEST_F(RouletteTableFixture, IgnoresBetsOfAPlayerInTheRoom) {
  join(FIRST_TABLE_SEAT);

  bet(FIRST_TABLE_SEAT,
      {RouletteBet{.kind = RouletteBetKind::Red,
                   .firstNumber = 0,
                   .amount = HUNDRED_CHIPS}},
      true);
  sit(FIRST_TABLE_SEAT);

  ASSERT_TRUE(listedSeat(FIRST_TABLE_SEAT).has_value());
  EXPECT_EQ(listedSeat(FIRST_TABLE_SEAT)->stake, 0U);
}

/**
 * Given a wheel that turns with 100 chips on red
 * When the player sends other bets for that round
 * Then they are ignored: the bets are closed
 */
TEST_F(RouletteTableFixture, IgnoresBetsOnceTheWheelTurns) {
  join(FIRST_TABLE_SEAT);
  sit(FIRST_TABLE_SEAT);
  bet(FIRST_TABLE_SEAT,
      {RouletteBet{.kind = RouletteBetKind::Red,
                   .firstNumber = 0,
                   .amount = HUNDRED_CHIPS}},
      true);
  runTicks(1);

  bet(FIRST_TABLE_SEAT,
      {RouletteBet{.kind = RouletteBetKind::Black,
                   .firstNumber = 0,
                   .amount = HALF_THE_CAP}},
      true);

  ASSERT_TRUE(listedSeat(FIRST_TABLE_SEAT).has_value());
  EXPECT_EQ(listedSeat(FIRST_TABLE_SEAT)->stake, HUNDRED_CHIPS);
}

/**
 * Given a wheel that turns
 * When it has turned for its whole time, then the result has shown for its
 * whole time
 * Then the result shows, then the bets of the next round open, emptied
 */
TEST_F(RouletteTableFixture, ShowsTheResultThenOpensTheNextRound) {
  join(FIRST_TABLE_SEAT);
  sit(FIRST_TABLE_SEAT);
  bet(FIRST_TABLE_SEAT,
      {RouletteBet{.kind = RouletteBetKind::Red,
                   .firstNumber = 0,
                   .amount = HUNDRED_CHIPS}},
      true);
  runTicks(1);
  const std::uint16_t firstRound = table().round().round;

  runTicks(ROULETTE_SPINNING_TICKS);
  ASSERT_EQ(table().round().phase, RoulettePhase::Result);
  runTicks(ROULETTE_RESULT_TICKS);

  EXPECT_EQ(table().round().phase, RoulettePhase::Betting);
  EXPECT_EQ(table().round().round, firstRound + 1);
  ASSERT_TRUE(listedSeat(FIRST_TABLE_SEAT).has_value());
  EXPECT_EQ(listedSeat(FIRST_TABLE_SEAT)->stake, 0U);
}

/**
 * Given a player who staked 100 straight on the number drawn
 * When it goes back to the room while the wheel turns
 * Then the round still lists its payout, until the next round opens
 */
TEST_F(RouletteTableFixture, ListsAPlayerWhoLeftUntilItsRoundEnds) {
  join(FIRST_TABLE_SEAT);
  sit(FIRST_TABLE_SEAT);
  bet(FIRST_TABLE_SEAT,
      {RouletteBet{.kind = RouletteBetKind::Straight,
                   .firstNumber = SCRIPTED_NUMBER,
                   .amount = HUNDRED_CHIPS}},
      true);
  runTicks(1);

  standUp(FIRST_TABLE_SEAT);
  ASSERT_TRUE(listedSeat(FIRST_TABLE_SEAT).has_value());
  EXPECT_EQ(listedSeat(FIRST_TABLE_SEAT)->payout, STRAIGHT_RETURN_OF_A_HUNDRED);
  runTicks(ROULETTE_SPINNING_TICKS + ROULETTE_RESULT_TICKS);

  EXPECT_FALSE(listedSeat(FIRST_TABLE_SEAT).has_value());
}

/**
 * Given a seated player with 100 chips on red, whose session closes
 * When another player, from another address, takes the freed seat
 * Then the newcomer is not at the casino and the round does not list it
 */
TEST_F(RouletteTableFixture, ANewPlayerOnAFreedSeatStartsAfresh) {
  const std::uint8_t freedSeat = join(FIRST_TABLE_SEAT).playerNumber;
  sit(FIRST_TABLE_SEAT);
  bet(FIRST_TABLE_SEAT,
      {RouletteBet{.kind = RouletteBetKind::Red,
                   .firstNumber = 0,
                   .amount = HUNDRED_CHIPS}},
      false);
  leave(FIRST_TABLE_SEAT);

  ASSERT_EQ(join(SECOND_TABLE_SEAT).playerNumber, freedSeat);

  EXPECT_FALSE(player(SECOND_TABLE_SEAT).atCasino);
  EXPECT_FALSE(listedSeat(SECOND_TABLE_SEAT).has_value());
}

/**
 * Given a seated player, a few rounds into the table
 * When the table closes as the game leaves its waiting room
 * Then the player is back in the room, the round lists nobody and counts from
 * 0 again
 */
TEST_F(RouletteTableFixture, ClosingSendsEveryoneBackToTheRoom) {
  join(FIRST_TABLE_SEAT);
  sit(FIRST_TABLE_SEAT);
  bet(FIRST_TABLE_SEAT, {}, true);
  runTicks(1 + ROULETTE_SPINNING_TICKS + ROULETTE_RESULT_TICKS);
  ASSERT_NE(table().round().round, 0U);

  table().close();

  EXPECT_FALSE(player(FIRST_TABLE_SEAT).atCasino);
  EXPECT_TRUE(table().round().seats.empty());
  EXPECT_EQ(table().round().round, 0U);
}

}  // namespace rtype::server::testing

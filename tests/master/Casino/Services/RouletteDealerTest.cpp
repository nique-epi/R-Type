#include <gtest/gtest.h>
#include <optional>
#include <variant>
#include <vector>
#include "BetRefusal.hpp"
#include "BetsReceipt.hpp"
#include "DrawnRound.hpp"
#include "RoomRound.hpp"
#include "RouletteDealerFixture.hpp"
#include "RouletteDealerTestConstants.hpp"
#include "RoundBets.hpp"

using rtype::master::BetRefusal;
using rtype::master::DrawnRound;
using rtype::network::BetsReceipt;
using rtype::network::RoomRound;
using rtype::network::RoundBets;

/**
 * Given the bets of every room open
 * When the dealer looks one millisecond before they close, then as they close
 * Then it draws nothing first, then the round of each room, one number each
 */
TEST_F(RouletteDealerFixture, DrawsEachRoomOnceItsBetsClose) {
  EXPECT_TRUE(drawAt(BET_ROUND_CLOSE - ONE_TICK).empty());

  const std::vector<DrawnRound> drawn = drawAt(BET_ROUND_CLOSE);

  ASSERT_EQ(drawn.size(), DEALER_TEST_ROOMS);
  EXPECT_EQ(drawn.front().place.round, BET_ROUND);
  EXPECT_EQ(draw().drawCount(), DEALER_TEST_ROOMS);
}

/**
 * Given rounds drawn as their bets closed
 * When the dealer looks again a pass later, in the same phase
 * Then it draws nothing more
 */
TEST_F(RouletteDealerFixture, DrawsARoundOnce) {
  static_cast<void>(drawAt(BET_ROUND_CLOSE));

  EXPECT_TRUE(drawAt(BET_ROUND_CLOSE + ONE_PASS).empty());
  EXPECT_EQ(draw().drawCount(), DEALER_TEST_ROOMS);
}

/**
 * Given ACE seated, with 10 chips on 19 and 25 on red
 * When the bets close and 19 comes out
 * Then ACE gets 360 for the straight and 50 for red, once, even when the
 * dealer looks again
 */
TEST_F(RouletteDealerFixture, PaysTheWinnersOnce) {
  ASSERT_TRUE(std::holds_alternative<BetsReceipt>(aceBetsAt(BET_ROUND_START)));
  ASSERT_EQ(chipsOfAce(), CHIPS_ONCE_STAKED);

  static_cast<void>(drawAt(BET_ROUND_CLOSE));
  static_cast<void>(drawAt(BET_ROUND_CLOSE + ONE_PASS));

  EXPECT_EQ(chipsOfAce(), CHIPS_ONCE_BOTH_WON);
  const RoundBets bets =
      casino().betsOf(RoomRound{.room = BET_ROOM, .round = BET_ROUND}, ace());
  EXPECT_EQ(bets.winningNumber, std::optional{RED_NINETEEN});
}

/**
 * Given ACE's bets on a round whose bets closed while no dealer looked
 * When the dealer looks for missed rounds three rounds later, while the bets
 * of that round are open
 * Then it draws the round of ACE alone, and pays it
 */
TEST_F(RouletteDealerFixture, DrawsARoundItMissedOnceItLooksAgain) {
  ASSERT_TRUE(std::holds_alternative<BetsReceipt>(aceBetsAt(BET_ROUND_START)));

  const std::vector<DrawnRound> drawn =
      drawMissedAt(BET_ROUND_START + (DEALER_TEST_CYCLE * 3));

  ASSERT_EQ(drawn.size(), 1U);
  EXPECT_EQ(drawn.front().place.round, BET_ROUND);
  EXPECT_EQ(drawn.front().place.room, BET_ROOM);
  EXPECT_EQ(chipsOfAce(), CHIPS_ONCE_BOTH_WON);
}

/**
 * Given ACE's bets on a round whose bets closed while no dealer looked
 * When the dealer makes its pass of every interval three rounds later, while
 * the bets of that round are open
 * Then it draws nothing: only the pass after a start or a failure looks back
 */
TEST_F(RouletteDealerFixture, TheIntervalPassLeavesAMissedRound) {
  ASSERT_TRUE(std::holds_alternative<BetsReceipt>(aceBetsAt(BET_ROUND_START)));

  EXPECT_TRUE(drawAt(BET_ROUND_START + (DEALER_TEST_CYCLE * 3)).empty());
  EXPECT_EQ(chipsOfAce(), CHIPS_ONCE_STAKED);
}

/**
 * Given a round the dealer drew as soon as its bets closed
 * When ACE sends bets for that round with a date before the close, as a
 * request that waited for the lock of the database would
 * Then the bets are refused as closed, and no chip moves
 */
TEST_F(RouletteDealerFixture, RefusesBetsOnADrawnRound) {
  static_cast<void>(drawAt(BET_ROUND_CLOSE));

  const auto refused = aceBetsAt(BET_ROUND_CLOSE - ONE_TICK);

  ASSERT_TRUE(std::holds_alternative<BetRefusal>(refused));
  EXPECT_EQ(std::get<BetRefusal>(refused), BetRefusal::BetsClosed);
  EXPECT_EQ(chipsOfAce(), CHIPS_AT_OPENING);
}

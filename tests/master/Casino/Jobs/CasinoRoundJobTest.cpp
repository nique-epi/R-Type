#include <gtest/gtest.h>
#include <thread>
#include "CasinoRoundJob.hpp"
#include "CasinoRoundJobTestConstants.hpp"
#include "RouletteDealerFixture.hpp"
#include "RouletteDealerTestConstants.hpp"
#include "RoundClock.hpp"

using rtype::master::CasinoRoundJob;
using rtype::master::RoundClock;

/**
 * Given a job whose clock stands where the bets of the round close, and which
 * looks for rounds every millisecond
 * When it runs until both rooms show a number, then stops
 * Then each room was drawn once, however many passes it made
 */
TEST_F(RouletteDealerFixture, TheJobDrawsEachRoundDueOnItsClockOnce) {
  {
    const CasinoRoundJob job{dealer(), DEALER_TEST_SETTINGS,
                             RoundClock{.now = [] { return BET_ROUND_CLOSE; },
                                        .interval = QUICK_PASS}};
    for (int poll = 0;
         poll < DRAWN_POLLS &&
         !casino().roomState(SECOND_ROOM, BET_ROUND_CLOSE).winningNumber;
         ++poll) {
      std::this_thread::sleep_for(DRAWN_POLL);
    }
    std::this_thread::sleep_for(DRAWN_POLL);
  }

  EXPECT_TRUE(casino().roomState(BET_ROOM, BET_ROUND_CLOSE).winningNumber);
  EXPECT_TRUE(casino().roomState(SECOND_ROOM, BET_ROUND_CLOSE).winningNumber);
  EXPECT_EQ(draw().drawCount(), DEALER_TEST_ROOMS);
}

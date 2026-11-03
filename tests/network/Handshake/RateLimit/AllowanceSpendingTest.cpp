#include <gtest/gtest.h>
#include <vector>
#include "AllowanceSpending.hpp"
#include "AllowanceVerdict.hpp"
#include "RateLimitFixtures.hpp"
#include "RateLimitTestConstants.hpp"
#include "SenderAllowance.hpp"

using rtype::network::AllowanceVerdict;
using rtype::network::SenderAllowance;
using rtype::network::spendAllowance;
using rtype::network::testing::bannedAtStart;
using rtype::network::testing::bannedAtStartUnder;
using rtype::network::testing::burstSpentAtStart;
using rtype::network::testing::FLOOD_DATAGRAM_COUNT;
using rtype::network::testing::HALFWAY_THROUGH_THE_BAN;
using rtype::network::testing::LONG_SILENCE;
using rtype::network::testing::ONE_NANOSECOND;
using rtype::network::testing::ONE_WINDOW;
using rtype::network::testing::SHORT_BAN_DURATION;
using rtype::network::testing::SHORT_BAN_RATE_LIMIT;
using rtype::network::testing::spendBurstUnder;
using rtype::network::testing::spendEveryInterval;
using rtype::network::testing::spendTimes;
using rtype::network::testing::START;
using rtype::network::testing::STEADY_DATAGRAM_COUNT;
using rtype::network::testing::TEST_BAN_DURATION;
using rtype::network::testing::TEST_BURST;
using rtype::network::testing::TEST_DROPS_BEFORE_BAN;
using rtype::network::testing::TEST_INTERVAL;
using rtype::network::testing::TEST_RATE_LIMIT;

/**
 * Given a sender that never sent anything
 * When its first datagram comes, at the origin of the clock
 * Then it is allowed
 */
TEST(AllowanceSpending, AllowsTheFirstDatagramOfASender) {
  SenderAllowance allowance;

  EXPECT_EQ(spendAllowance(allowance, TEST_RATE_LIMIT, START),
            AllowanceVerdict::Allowed);
}

/**
 * Given a sender that never sent anything, under a limit of three at once
 * When it sends three datagrams at the same time
 * Then all three are allowed
 */
TEST(AllowanceSpending, AllowsAWholeBurstAtOnce) {
  SenderAllowance allowance;

  EXPECT_EQ(spendTimes(allowance, TEST_BURST, START),
            (std::vector{AllowanceVerdict::Allowed, AllowanceVerdict::Allowed,
                         AllowanceVerdict::Allowed}));
}

/**
 * Given a sender that just spent its whole burst
 * When it sends one more datagram at the same time
 * Then that datagram is dropped over the cap
 */
TEST(AllowanceSpending, DropsTheDatagramPastTheBurst) {
  SenderAllowance allowance = burstSpentAtStart();

  EXPECT_EQ(spendAllowance(allowance, TEST_RATE_LIMIT, START),
            AllowanceVerdict::OverCap);
}

/**
 * Given a sender that spent its whole burst at the start
 * When it sends a datagram one interval later
 * Then that datagram is allowed
 */
TEST(AllowanceSpending, AllowsADatagramOneIntervalAfterTheBurst) {
  SenderAllowance allowance = burstSpentAtStart();

  EXPECT_EQ(spendAllowance(allowance, TEST_RATE_LIMIT, START + TEST_INTERVAL),
            AllowanceVerdict::Allowed);
}

/**
 * Given a sender that spent its whole burst at the start
 * When it sends a datagram a nanosecond before one interval has passed
 * Then that datagram is dropped over the cap
 */
TEST(AllowanceSpending, DropsADatagramJustBeforeTheNextInterval) {
  SenderAllowance allowance = burstSpentAtStart();

  EXPECT_EQ(spendAllowance(allowance, TEST_RATE_LIMIT,
                           START + TEST_INTERVAL - ONE_NANOSECOND),
            AllowanceVerdict::OverCap);
}

/**
 * Given a sender that sends one datagram every interval
 * When it has sent a hundred of them
 * Then every one was allowed
 */
TEST(AllowanceSpending, NeverDropsASenderThatKeepsToTheInterval) {
  SenderAllowance allowance;

  EXPECT_EQ(spendEveryInterval(allowance, STEADY_DATAGRAM_COUNT),
            std::vector<AllowanceVerdict>(STEADY_DATAGRAM_COUNT,
                                          AllowanceVerdict::Allowed));
}

/**
 * Given a sender that spent its burst, then stayed silent ten seconds
 * When it sends four datagrams at once
 * Then three are allowed and the fourth is dropped: a silence gives back one
 * burst, never more
 */
TEST(AllowanceSpending, GivesBackNoMoreThanABurstAfterALongSilence) {
  SenderAllowance allowance = burstSpentAtStart();

  EXPECT_EQ(
      spendTimes(allowance, TEST_BURST + 1, START + LONG_SILENCE),
      (std::vector{AllowanceVerdict::Allowed, AllowanceVerdict::Allowed,
                   AllowanceVerdict::Allowed, AllowanceVerdict::OverCap}));
}

/**
 * Given a sender that spent its burst, under a limit that bans on the second
 * drop within a second
 * When it sends two more datagrams at once
 * Then the first is dropped over the cap and the second starts a ban
 */
TEST(AllowanceSpending, BansOnTheDropThatReachesTheLimit) {
  SenderAllowance allowance = burstSpentAtStart();

  EXPECT_EQ(
      spendTimes(allowance, TEST_DROPS_BEFORE_BAN, START),
      (std::vector{AllowanceVerdict::OverCap, AllowanceVerdict::NewlyBanned}));
}

/**
 * Given a sender dropped once at the start, which opened a window of a second
 * When, one second later, it spends a burst and sends one datagram more
 * Then that datagram is only dropped: the window of the first drop had ended
 */
TEST(AllowanceSpending, CountsTheDropsOfEachWindowApart) {
  SenderAllowance allowance = burstSpentAtStart();
  static_cast<void>(spendAllowance(allowance, TEST_RATE_LIMIT, START));

  const std::vector<AllowanceVerdict> verdicts =
      spendTimes(allowance, TEST_BURST + 1, START + ONE_WINDOW);

  EXPECT_EQ(verdicts.back(), AllowanceVerdict::OverCap);
}

/**
 * Given a sender banned at the start for ten seconds
 * When it sends a datagram a nanosecond before the end of the ban
 * Then that datagram is dropped as banned
 */
TEST(AllowanceSpending, DropsEveryDatagramUntilTheBanEnds) {
  SenderAllowance allowance = bannedAtStart();

  EXPECT_EQ(spendAllowance(allowance, TEST_RATE_LIMIT,
                           START + TEST_BAN_DURATION - ONE_NANOSECOND),
            AllowanceVerdict::Banned);
}

/**
 * Given a sender banned at the start, which floods halfway through its ban
 * When the ten seconds of the ban have passed
 * Then its next datagram is allowed: flooding did not lengthen the ban
 */
TEST(AllowanceSpending, DoesNotLengthenTheBanOfASenderThatKeepsFlooding) {
  SenderAllowance allowance = bannedAtStart();
  static_cast<void>(spendTimes(allowance, FLOOD_DATAGRAM_COUNT,
                               START + HALFWAY_THROUGH_THE_BAN));

  EXPECT_EQ(
      spendAllowance(allowance, TEST_RATE_LIMIT, START + TEST_BAN_DURATION),
      AllowanceVerdict::Allowed);
}

/**
 * Given a sender banned at the start
 * When its ban ends and it sends three datagrams at once
 * Then all three are allowed: the ban ends with a whole burst
 */
TEST(AllowanceSpending, GivesAWholeBurstBackWhenTheBanEnds) {
  SenderAllowance allowance = bannedAtStart();

  EXPECT_EQ(spendTimes(allowance, TEST_BURST, START + TEST_BAN_DURATION),
            (std::vector{AllowanceVerdict::Allowed, AllowanceVerdict::Allowed,
                         AllowanceVerdict::Allowed}));
}

/**
 * Given a sender banned at the start for 100 ms, less than the 300 ms its
 * burst takes to come back
 * When the ban ends and it sends three datagrams at once
 * Then all three are allowed: a ban forgets what the sender spent
 */
TEST(AllowanceSpending, GivesAWholeBurstBackWhenABanShorterThanItsRefillEnds) {
  SenderAllowance allowance = bannedAtStartUnder(SHORT_BAN_RATE_LIMIT);

  EXPECT_EQ(spendBurstUnder(allowance, SHORT_BAN_RATE_LIMIT,
                            START + SHORT_BAN_DURATION),
            (std::vector{AllowanceVerdict::Allowed, AllowanceVerdict::Allowed,
                         AllowanceVerdict::Allowed}));
}

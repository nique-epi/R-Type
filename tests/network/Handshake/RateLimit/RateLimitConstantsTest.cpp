#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>
#include <vector>
#include "AllowanceVerdict.hpp"
#include "ProtocolConstants.hpp"
#include "RateLimitConstants.hpp"
#include "RateLimitConstantsFixtures.hpp"
#include "RateLimitConstantsTestConstants.hpp"
#include "RateLimitTestConstants.hpp"
#include "TimeTestConstants.hpp"

using rtype::network::AllowanceVerdict;
using rtype::network::CONNECTION_ATTEMPT_TIMEOUT;
using rtype::network::PLAYER_INPUTS_PER_SECOND;
using rtype::network::PLAYER_RATE_LIMIT;
using rtype::network::REQUEST_RATE_LIMIT;
using rtype::network::testing::allowedCount;
using rtype::network::testing::burstThenOneAt;
using rtype::network::testing::FLOOD_DATAGRAM_COUNT;
using rtype::network::testing::joiningClientTimes;
using rtype::network::testing::LONG_RUN_DURATION;
using rtype::network::testing::playerTimes;
using rtype::network::testing::PROTOCOL_BAN_DURATION;
using rtype::network::testing::PROTOCOL_PLAYER_REFILL;
using rtype::network::testing::PROTOCOL_REQUEST_REFILL;
using rtype::network::testing::stalledClientTimes;
using rtype::network::testing::verdictsAt;

/**
 * Given a client that sends a request every 500 ms and answers each challenge
 * When it does so for the 5 s before it gives up
 * Then every request and every answer is allowed
 */
TEST(RateLimitConstants, LetsAJoiningClientThroughUntilItGivesUp) {
  const std::vector<std::chrono::nanoseconds> times =
      joiningClientTimes(CONNECTION_ATTEMPT_TIMEOUT);

  EXPECT_EQ(allowedCount(verdictsAt(REQUEST_RATE_LIMIT, times)), times.size());
}

/**
 * Given a client that never gives up, and sends a request every 500 ms and
 * answers each challenge
 * When it does so for ten minutes
 * Then every request and every answer is allowed: the pace of the protocol
 * never exhausts the limit, however long it lasts
 */
TEST(RateLimitConstants, LetsAClientThatKeepsToThePaceOfTheProtocolThrough) {
  const std::vector<std::chrono::nanoseconds> times =
      joiningClientTimes(LONG_RUN_DURATION);

  EXPECT_EQ(allowedCount(verdictsAt(REQUEST_RATE_LIMIT, times)), times.size());
}

/**
 * Given a client whose server stalls until just before the client gives up
 * When it sends a request every 500 ms for 5 s, then answers the challenge of
 * each of them at once, a round trip after the last one
 * Then every request and every answer is allowed
 */
TEST(RateLimitConstants, LetsTheAnswersOfAClientThroughAfterAStall) {
  const std::vector<std::chrono::nanoseconds> times = stalledClientTimes();

  EXPECT_EQ(allowedCount(verdictsAt(REQUEST_RATE_LIMIT, times)), times.size());
}

/**
 * Given a sender that sends a thousand requests at once, far more than a
 * client sends in a whole attempt
 * When its requests are spent
 * Then one of them starts a ban
 */
TEST(RateLimitConstants, BansASenderThatFloodsRequests) {
  const std::vector<std::chrono::nanoseconds> times(FLOOD_DATAGRAM_COUNT);

  const std::vector<AllowanceVerdict> verdicts =
      verdictsAt(REQUEST_RATE_LIMIT, times);

  EXPECT_EQ(std::ranges::count(verdicts, AllowanceVerdict::NewlyBanned), 1);
}

/**
 * Given a player that sends 60 inputs a second and a heartbeat every 500 ms
 * When it does so for ten minutes
 * Then every datagram is allowed: the pace of the protocol never exhausts the
 * limit, however long the game lasts
 */
TEST(RateLimitConstants, LetsEveryDatagramOfAPlayerThrough) {
  const std::vector<std::chrono::nanoseconds> times =
      playerTimes(LONG_RUN_DURATION);

  EXPECT_EQ(allowedCount(verdictsAt(PLAYER_RATE_LIMIT, times)), times.size());
}

/**
 * Given a player whose network delivers a second of its inputs together
 * When the sixty inputs are spent at once
 * Then all sixty are allowed
 */
TEST(RateLimitConstants, LetsASecondOfInputsThroughAtOnce) {
  const std::vector<std::chrono::nanoseconds> times(PLAYER_INPUTS_PER_SECOND);

  EXPECT_EQ(allowedCount(verdictsAt(PLAYER_RATE_LIMIT, times)),
            PLAYER_INPUTS_PER_SECOND);
}

/**
 * Given a sender that spent the whole burst of the requests at once
 * When it sends one more request a nanosecond before 250 ms passed
 * Then that request is dropped: the limit gives back no more than four
 * requests a second
 */
TEST(RateLimitConstants, GivesBackNoMoreThanFourRequestsASecond) {
  const std::vector<AllowanceVerdict> verdicts =
      verdictsAt(REQUEST_RATE_LIMIT,
                 burstThenOneAt(REQUEST_RATE_LIMIT,
                                PROTOCOL_REQUEST_REFILL - ONE_NANOSECOND));

  EXPECT_EQ(verdicts.back(), AllowanceVerdict::OverCap);
}

/**
 * Given a player that spent its whole burst at once
 * When it sends one more datagram a nanosecond before 1/120 s passed
 * Then that datagram is dropped: the limit gives back no more than 120
 * datagrams of a player a second
 */
TEST(RateLimitConstants, GivesBackNoMoreThanAHundredAndTwentyDatagramsASecond) {
  const std::vector<AllowanceVerdict> verdicts =
      verdictsAt(PLAYER_RATE_LIMIT,
                 burstThenOneAt(PLAYER_RATE_LIMIT,
                                PROTOCOL_PLAYER_REFILL - ONE_NANOSECOND));

  EXPECT_EQ(verdicts.back(), AllowanceVerdict::OverCap);
}

/**
 * Given the ban of the protocol page
 * When the ban of either limit is read
 * Then both last 10 s
 */
TEST(RateLimitConstants, BanForTheDurationOfTheProtocolPage) {
  EXPECT_EQ(REQUEST_RATE_LIMIT.banDuration, PROTOCOL_BAN_DURATION);
  EXPECT_EQ(PLAYER_RATE_LIMIT.banDuration, PROTOCOL_BAN_DURATION);
}

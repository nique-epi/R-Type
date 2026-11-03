#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>
#include <vector>
#include "AllowanceVerdict.hpp"
#include "RateLimitConstants.hpp"
#include "RateLimitConstantsFixtures.hpp"
#include "RateLimitConstantsTestConstants.hpp"

using rtype::network::AllowanceVerdict;
using rtype::network::PLAYER_RATE_LIMIT;
using rtype::network::REQUEST_RATE_LIMIT;
using rtype::network::testing::allowedCount;
using rtype::network::testing::ANSWERS_AFTER_A_STALL;
using rtype::network::testing::CLIENT_GIVE_UP;
using rtype::network::testing::INPUTS_AT_ONCE;
using rtype::network::testing::joiningClientTimes;
using rtype::network::testing::LONG_RUN_DURATION;
using rtype::network::testing::playerTimes;
using rtype::network::testing::PROTOCOL_BAN_DURATION;
using rtype::network::testing::REQUEST_FLOOD_COUNT;
using rtype::network::testing::verdictsAt;

/**
 * Given a client that sends a request every 500 ms and answers each challenge
 * When it does so for the 5 s before it gives up
 * Then every request and every answer is allowed
 */
TEST(RateLimitConstants, LetsAJoiningClientThroughUntilItGivesUp) {
  const std::vector<std::chrono::nanoseconds> times =
      joiningClientTimes(CLIENT_GIVE_UP);

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
 * Given a client whose challenges piled up during a stall of the server
 * When it sends its ten answers in the same tick
 * Then all ten are allowed
 */
TEST(RateLimitConstants, LetsTheAnswersOfAClientThroughAfterAStall) {
  const std::vector<std::chrono::nanoseconds> times(ANSWERS_AFTER_A_STALL);

  EXPECT_EQ(allowedCount(verdictsAt(REQUEST_RATE_LIMIT, times)),
            ANSWERS_AFTER_A_STALL);
}

/**
 * Given a sender that sends at once ten times the requests of a client in a
 * second
 * When its requests are spent
 * Then one of them starts a ban
 */
TEST(RateLimitConstants, BansASenderThatFloodsRequests) {
  const std::vector<std::chrono::nanoseconds> times(REQUEST_FLOOD_COUNT);

  const std::vector<AllowanceVerdict> verdicts =
      verdictsAt(REQUEST_RATE_LIMIT, times);

  EXPECT_EQ(std::ranges::count(verdicts, AllowanceVerdict::NewlyBanned), 1);
}

/**
 * Given a player that sends 60 inputs and a heartbeat every second
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
  const std::vector<std::chrono::nanoseconds> times(INPUTS_AT_ONCE);

  EXPECT_EQ(allowedCount(verdictsAt(PLAYER_RATE_LIMIT, times)), INPUTS_AT_ONCE);
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

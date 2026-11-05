#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include "AllowanceVerdict.hpp"
#include "HandshakeTestConstants.hpp"
#include "RateLimitFixtures.hpp"
#include "RateLimitTestConstants.hpp"
#include "RequestAllowances.hpp"
#include "TimeTestConstants.hpp"

using rtype::network::AllowanceVerdict;
using rtype::network::RequestAllowances;
using rtype::network::testing::fillARowWithStaggeredBans;
using rtype::network::testing::FORGED_SENDER_COUNT;
using rtype::network::testing::forgedSender;
using rtype::network::testing::limitedSender;
using rtype::network::testing::OTHER_SENDER_PORT;
using rtype::network::testing::ROW_SENDER_BANNED_FIRST;
using rtype::network::testing::ROW_SENDER_BANNED_LAST;
using rtype::network::testing::spendFromSender;
using rtype::network::testing::spendOnceFromEachForged;
using rtype::network::testing::START;
using rtype::network::testing::TEST_BURST;
using rtype::network::testing::TEST_DROPS_BEFORE_BAN;
using rtype::network::testing::TEST_KEY;
using rtype::network::testing::TEST_RATE_LIMIT;
using rtype::network::testing::TIME_BETWEEN_BANS;

/**
 * Given a sender that spent its whole burst
 * When it sends one more request
 * Then the table remembered what it spent: the request is dropped
 */
TEST(RequestAllowances, KeepsWhatASenderSpentBetweenTwoRequests) {
  RequestAllowances allowances{TEST_RATE_LIMIT, TEST_KEY};

  const std::vector<AllowanceVerdict> verdicts =
      spendFromSender(allowances, limitedSender(), TEST_BURST + 1);

  EXPECT_EQ(verdicts.back(), AllowanceVerdict::OverCap);
}

/**
 * Given a sender that spent its whole burst
 * When the same address sends from another port
 * Then that request is allowed: each port has its own allowance
 */
TEST(RequestAllowances, GivesEachPortOfAnAddressItsOwnAllowance) {
  RequestAllowances allowances{TEST_RATE_LIMIT, TEST_KEY};
  static_cast<void>(spendFromSender(allowances, limitedSender(), TEST_BURST));

  EXPECT_EQ(allowances.spend(limitedSender(OTHER_SENDER_PORT), START),
            AllowanceVerdict::Allowed);
}

/**
 * Given a banned sender
 * When ten thousand forged senders send one request each; under TEST_KEY
 * every row gets 21 to 58 of them, more than its 4 slots
 * Then the banned sender is still banned: forged senders replace each other,
 * not it
 */
TEST(RequestAllowances, KeepsABannedSenderBannedWhileForgedSendersPassBy) {
  RequestAllowances allowances{TEST_RATE_LIMIT, TEST_KEY};
  static_cast<void>(spendFromSender(allowances, limitedSender(),
                                    TEST_BURST + TEST_DROPS_BEFORE_BAN));

  spendOnceFromEachForged(allowances, FORGED_SENDER_COUNT, START);

  EXPECT_EQ(allowances.spend(limitedSender(), START), AllowanceVerdict::Banned);
}

/**
 * Given a sender that spent its burst and is not banned; under TEST_KEY its
 * row, 221, gets 44 of the forged senders below
 * When ten thousand forged senders send one request each, a nanosecond later
 * Then the sender was replaced in its row and starts again with a whole burst
 */
TEST(RequestAllowances, ReplacesASenderNotBannedOnceItsRowFills) {
  RequestAllowances allowances{TEST_RATE_LIMIT, TEST_KEY};
  static_cast<void>(
      spendFromSender(allowances, limitedSender(), TEST_BURST + 1));

  spendOnceFromEachForged(allowances, FORGED_SENDER_COUNT,
                          START + ONE_NANOSECOND);

  EXPECT_EQ(allowances.spend(limitedSender(), START + ONE_NANOSECOND),
            AllowanceVerdict::Allowed);
}

/**
 * Given a row whose four slots hold senders banned at different times, the
 * one in its second slot first, then a fifth sender of that row
 * When the sender whose ban ends first sends again
 * Then its request is allowed: the fifth sender took its slot, so it starts
 * again with a whole burst
 */
TEST(RequestAllowances, GivesUpTheBanThatEndsFirstOnceEverySlotOfARowIsBanned) {
  RequestAllowances allowances{TEST_RATE_LIMIT, TEST_KEY};
  const std::chrono::nanoseconds filled = fillARowWithStaggeredBans(allowances);

  EXPECT_EQ(allowances.spend(forgedSender(ROW_SENDER_BANNED_FIRST),
                             filled + TIME_BETWEEN_BANS),
            AllowanceVerdict::Allowed);
}

/**
 * Given a row whose four slots hold senders banned at different times, then a
 * fifth sender of that row
 * When the sender banned last sends again
 * Then it is still banned: only the ban that ends first was given up
 */
TEST(RequestAllowances, KeepsTheOtherBansOfARowFullOfBannedSenders) {
  RequestAllowances allowances{TEST_RATE_LIMIT, TEST_KEY};
  const std::chrono::nanoseconds filled = fillARowWithStaggeredBans(allowances);

  EXPECT_EQ(allowances.spend(forgedSender(ROW_SENDER_BANNED_LAST),
                             filled + TIME_BETWEEN_BANS),
            AllowanceVerdict::Banned);
}

#include <gtest/gtest.h>
#include "ReadyRule.hpp"
#include "ReadyRuleTestConstants.hpp"

using rtype::tools::asksReady;
using rtype::tools::testing::A_BOT_STILL_CONNECTING;
using rtype::tools::testing::ACCEPTED_AND_WAITING;
using rtype::tools::testing::ALREADY_ASKED;
using rtype::tools::testing::EVERY_BOT_ANSWERED;
using rtype::tools::testing::IN_A_GAME;
using rtype::tools::testing::REFUSED;

/**
 * Given an accepted bot in a waiting room, that has not asked yet
 * When every bot of the squad has the answer to its connection
 * Then it asks to be ready
 */
TEST(ReadyRule, AnAcceptedBotAsksOnceEveryBotHasItsAnswer) {
  EXPECT_TRUE(asksReady(ACCEPTED_AND_WAITING, EVERY_BOT_ANSWERED));
}

/**
 * Given an accepted bot in a waiting room, that has not asked yet
 * When another bot of the squad still waits for its answer
 * Then it does not ask, so its game does not start before it is full
 */
TEST(ReadyRule, NoBotAsksWhileAnotherStillConnects) {
  EXPECT_FALSE(asksReady(ACCEPTED_AND_WAITING, A_BOT_STILL_CONNECTING));
}

/**
 * Given a bot that already asked to be ready in this waiting room
 * When every bot has its answer
 * Then it does not ask again
 */
TEST(ReadyRule, ABotThatAskedDoesNotAskAgain) {
  EXPECT_FALSE(asksReady(ALREADY_ASKED, EVERY_BOT_ANSWERED));
}

/**
 * Given a bot whose game started
 * When every bot has its answer
 * Then it does not ask
 */
TEST(ReadyRule, ABotInAGameDoesNotAsk) {
  EXPECT_FALSE(asksReady(IN_A_GAME, EVERY_BOT_ANSWERED));
}

/**
 * Given a bot the server refused
 * When every bot has its answer
 * Then it does not ask
 */
TEST(ReadyRule, ARefusedBotDoesNotAsk) {
  EXPECT_FALSE(asksReady(REFUSED, EVERY_BOT_ANSWERED));
}

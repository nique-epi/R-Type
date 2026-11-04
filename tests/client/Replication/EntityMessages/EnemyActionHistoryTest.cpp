#include <gtest/gtest.h>
#include "EnemyActionHistory.hpp"
#include "EnemyActionHistoryTestConstants.hpp"
#include "GameEventHistoryTestConstants.hpp"
#include "NetworkTestConstants.hpp"
#include "TimeTestConstants.hpp"

using rtype::client::EnemyActionHistory;
using rtype::client::testing::EVENT_MEMORY_REQUIRED;
using rtype::client::testing::OTHER_SIGNAL;
using rtype::client::testing::SIGNAL;
using rtype::client::testing::STRIKE;

/**
 * Given a Signal seen, then the Strike that follows it
 * When a copy of the Signal arrives 100 ms later
 * Then it is not a first copy: the enemy is not put back in its Signal
 */
TEST(EnemyActionHistory, ALateCopyOfAnEarlierPhaseIsNotNew) {
  EnemyActionHistory history;
  ASSERT_TRUE(history.isFirstCopy(SIGNAL));
  ASSERT_TRUE(history.isFirstCopy(STRIKE));

  history.advance(PROTOCOL_RESEND_DELAY);

  EXPECT_FALSE(history.isFirstCopy(SIGNAL));
}

/**
 * Given a Signal seen
 * When the Signal of another attack of the same enemy arrives
 * Then it is new
 */
TEST(EnemyActionHistory, AnotherAttackIsNew) {
  EnemyActionHistory history;
  ASSERT_TRUE(history.isFirstCopy(SIGNAL));

  EXPECT_TRUE(history.isFirstCopy(OTHER_SIGNAL));
}

/**
 * Given a Signal seen
 * When it comes back one nanosecond less than two seconds later
 * Then it is still a copy
 */
TEST(EnemyActionHistory, AnActionIsRememberedForTwoSeconds) {
  EnemyActionHistory history;
  ASSERT_TRUE(history.isFirstCopy(SIGNAL));

  history.advance(EVENT_MEMORY_REQUIRED - ONE_NANOSECOND);

  EXPECT_FALSE(history.isFirstCopy(SIGNAL));
}

/**
 * Given a Signal seen
 * When it comes back two seconds later
 * Then it is new: the enemy attacks again
 */
TEST(EnemyActionHistory, AnActionIsForgottenAfterTwoSeconds) {
  EnemyActionHistory history;
  ASSERT_TRUE(history.isFirstCopy(SIGNAL));

  history.advance(EVENT_MEMORY_REQUIRED);

  EXPECT_TRUE(history.isFirstCopy(SIGNAL));
}

/**
 * Given a Signal seen in a session left
 * When the history is forgotten and the Signal comes again
 * Then it is new
 */
TEST(EnemyActionHistory, ForgettingClearsEveryAction) {
  EnemyActionHistory history;
  ASSERT_TRUE(history.isFirstCopy(SIGNAL));

  history.forget();

  EXPECT_TRUE(history.isFirstCopy(SIGNAL));
}

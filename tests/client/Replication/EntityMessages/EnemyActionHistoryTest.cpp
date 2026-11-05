#include <gtest/gtest.h>
#include "EnemyAction.hpp"
#include "EnemyActionHistory.hpp"
#include "GameEventHistoryTestConstants.hpp"
#include "NetworkTestConstants.hpp"
#include "ServerSessionTestConstants.hpp"
#include "TimeTestConstants.hpp"

using rtype::client::EnemyActionHistory;
using rtype::client::testing::EVENT_MEMORY_REQUIRED;
using rtype::network::EnemyAction;
using rtype::network::EnemyActionPhase;

namespace {

constexpr EnemyAction FIRST_STRIKE{
    .entityId = 7, .attack = 1, .phase = EnemyActionPhase::Strike};

}  // namespace

/**
 * Given an action already seen
 * When its next two copies come 100 ms apart
 * Then neither is a first copy
 */
TEST(EnemyActionHistory, TheOtherCopiesOfAnActionAreNotNew) {
  EnemyActionHistory history;
  ASSERT_TRUE(history.isFirstCopy(FIRST_STRIKE));

  history.advance(PROTOCOL_RESEND_DELAY);
  const bool second = history.isFirstCopy(FIRST_STRIKE);
  history.advance(PROTOCOL_RESEND_DELAY);
  const bool third = history.isFirstCopy(FIRST_STRIKE);

  EXPECT_FALSE(second);
  EXPECT_FALSE(third);
}

/**
 * Given the strike of the first attack of an enemy seen
 * When the same enemy enters another phase, another attack, and another enemy
 * enters the same phase
 * Then each is a first copy: the key is the entity, the attack and the phase
 */
TEST(EnemyActionHistory, EachEntityAttackAndPhaseIsItsOwnKey) {
  EnemyActionHistory history;
  ASSERT_TRUE(history.isFirstCopy(FIRST_STRIKE));
  EnemyAction otherPhase = FIRST_STRIKE;
  otherPhase.phase = EnemyActionPhase::Recover;
  EnemyAction otherAttack = FIRST_STRIKE;
  otherAttack.attack = 2;
  EnemyAction otherEntity = FIRST_STRIKE;
  otherEntity.entityId = 8;

  EXPECT_TRUE(history.isFirstCopy(otherPhase));
  EXPECT_TRUE(history.isFirstCopy(otherAttack));
  EXPECT_TRUE(history.isFirstCopy(otherEntity));
}

/**
 * Given an action seen
 * When it comes back one nanosecond less than two seconds later
 * Then it is still a copy
 */
TEST(EnemyActionHistory, AnActionIsRememberedForTwoSeconds) {
  EnemyActionHistory history;
  ASSERT_TRUE(history.isFirstCopy(FIRST_STRIKE));

  history.advance(EVENT_MEMORY_REQUIRED - ONE_NANOSECOND);

  EXPECT_FALSE(history.isFirstCopy(FIRST_STRIKE));
}

/**
 * Given an action seen
 * When it comes back two seconds later
 * Then it is new: the enemy attacked again
 */
TEST(EnemyActionHistory, AnActionIsForgottenAfterTwoSeconds) {
  EnemyActionHistory history;
  ASSERT_TRUE(history.isFirstCopy(FIRST_STRIKE));

  history.advance(EVENT_MEMORY_REQUIRED);

  EXPECT_TRUE(history.isFirstCopy(FIRST_STRIKE));
}

/**
 * Given an action seen in a session left
 * When the history is forgotten and the action comes again
 * Then it is new
 */
TEST(EnemyActionHistory, ForgettingClearsEveryAction) {
  EnemyActionHistory history;
  ASSERT_TRUE(history.isFirstCopy(FIRST_STRIKE));

  history.forget();

  EXPECT_TRUE(history.isFirstCopy(FIRST_STRIKE));
}

#include <gtest/gtest.h>
#include <optional>
#include "ActionCycle.hpp"
#include "ActionCycleFixtures.hpp"
#include "ActionCycleTestConstants.hpp"
#include "ActionKey.hpp"
#include "Intent.hpp"

using rtype::game::ActionKey;
using rtype::game::hasRested;
using rtype::game::Intent;
using rtype::game::withNextPhase;
using rtype::game::withStart;
using rtype::game::testing::FULL_ATTACK;
using rtype::game::testing::FULL_RECOVER;
using rtype::game::testing::FULL_SIGNAL;
using rtype::game::testing::FULL_STRIKE;
using rtype::game::testing::LONE_STRIKE;
using rtype::game::testing::LONG_REST;
using rtype::game::testing::ONE_TICK;
using rtype::game::testing::SHORT_REST;
using rtype::game::testing::STRIKE_ONLY_ATTACK;
using rtype::game::testing::viewIn;
using rtype::game::testing::WINDOW_REST_AFTER_FULL_ATTACK;

/**
 * Given an attack with a signal
 * When it is started
 * Then its signal starts
 */
TEST(ActionCycle, AnAttackStartsOnItsSignal) {
  EXPECT_EQ(withStart(Intent{}, FULL_ATTACK).startsAction, FULL_SIGNAL);
}

/**
 * Given an attack with a strike only
 * When it is started
 * Then its strike starts
 */
TEST(ActionCycle, AnAttackWithoutSignalStartsOnItsStrike) {
  EXPECT_EQ(withStart(Intent{}, STRIKE_ONLY_ATTACK).startsAction, LONE_STRIKE);
}

/**
 * Given a body one tick short of the end of its signal
 * When the cycle steps
 * Then nothing changes
 */
TEST(ActionCycle, APhaseGoesOnUntilItHasLastedItsTime) {
  const Intent intent = withNextPhase(
      Intent{}, viewIn(FULL_SIGNAL, FULL_ATTACK.signal - ONE_TICK),
      FULL_ATTACK);

  EXPECT_FALSE(intent.startsAction.has_value());
  EXPECT_FALSE(intent.endsAction);
}

/**
 * Given a body whose signal has lasted its time
 * When the cycle steps
 * Then the strike starts
 */
TEST(ActionCycle, TheStrikeFollowsTheSignal) {
  const Intent intent = withNextPhase(
      Intent{}, viewIn(FULL_SIGNAL, FULL_ATTACK.signal), FULL_ATTACK);

  EXPECT_EQ(intent.startsAction, FULL_STRIKE);
}

/**
 * Given a body whose strike has lasted its time, in an attack with a recover
 * When the cycle steps
 * Then the recover starts
 */
TEST(ActionCycle, TheRecoverFollowsTheStrike) {
  const Intent intent = withNextPhase(
      Intent{}, viewIn(FULL_STRIKE, FULL_ATTACK.strike), FULL_ATTACK);

  EXPECT_EQ(intent.startsAction, FULL_RECOVER);
}

/**
 * Given a body whose recover has lasted its time
 * When the cycle steps
 * Then the action ends and no phase starts
 */
TEST(ActionCycle, TheActionEndsAfterTheRecover) {
  const Intent intent = withNextPhase(
      Intent{}, viewIn(FULL_RECOVER, FULL_ATTACK.recover), FULL_ATTACK);

  EXPECT_TRUE(intent.endsAction);
  EXPECT_FALSE(intent.startsAction.has_value());
}

/**
 * Given a body whose strike has lasted its time, in an attack with no recover
 * When the cycle steps
 * Then the action ends
 */
TEST(ActionCycle, AnAttackWithoutRecoverEndsAfterItsStrike) {
  const Intent intent =
      withNextPhase(Intent{}, viewIn(LONE_STRIKE, STRIKE_ONLY_ATTACK.strike),
                    STRIKE_ONLY_ATTACK);

  EXPECT_TRUE(intent.endsAction);
}

/**
 * Given a body long into the strike of another attack
 * When the cycle of the full attack steps
 * Then nothing changes
 */
TEST(ActionCycle, AnotherAttackIsLeftAlone) {
  const Intent intent =
      withNextPhase(Intent{}, viewIn(LONE_STRIKE, LONG_REST), FULL_ATTACK);

  EXPECT_FALSE(intent.startsAction.has_value());
  EXPECT_FALSE(intent.endsAction);
}

/**
 * Given an idle body
 * When the cycle steps
 * Then nothing starts: the behaviour chooses when to attack
 */
TEST(ActionCycle, AnIdleBodyIsLeftAlone) {
  const Intent intent =
      withNextPhase(Intent{}, viewIn(std::nullopt, LONG_REST), FULL_ATTACK);

  EXPECT_FALSE(intent.startsAction.has_value());
}

/**
 * Given a body idle one tick short of what the repeat window asks after the
 * full attack, a rest shorter than that being asked
 * When it asks whether it has rested
 * Then it has not: the client would take its next signal for a copy
 */
TEST(ActionCycle, TheRepeatWindowLengthensAShortRest) {
  EXPECT_FALSE(
      hasRested(viewIn(std::nullopt, WINDOW_REST_AFTER_FULL_ATTACK - ONE_TICK),
                FULL_ATTACK, SHORT_REST));
}

/**
 * Given a body idle exactly as long as the repeat window asks after the full
 * attack
 * When it asks whether it has rested, with a shorter rest
 * Then it has
 */
TEST(ActionCycle, ABodyHasRestedOnceTheRepeatWindowIsBehind) {
  EXPECT_TRUE(hasRested(viewIn(std::nullopt, WINDOW_REST_AFTER_FULL_ATTACK),
                        FULL_ATTACK, SHORT_REST));
}

/**
 * Given a body idle past the repeat window but one tick short of its rest
 * When it asks whether it has rested
 * Then it has not
 */
TEST(ActionCycle, ALongRestIsKept) {
  EXPECT_FALSE(hasRested(viewIn(std::nullopt, LONG_REST - ONE_TICK),
                         FULL_ATTACK, LONG_REST));
}

/**
 * Given a body long into the recover of an attack
 * When it asks whether it has rested
 * Then it has not: only an idle body rests
 */
TEST(ActionCycle, ABodyInActionHasNotRested) {
  EXPECT_FALSE(
      hasRested(viewIn(FULL_RECOVER, LONG_REST), FULL_ATTACK, SHORT_REST));
}

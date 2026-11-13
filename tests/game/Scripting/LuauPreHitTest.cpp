#include <gtest/gtest.h>
#include <cstddef>
#include "BehaviourScriptsTestConstants.hpp"
#include "EngineException.hpp"
#include "Intent.hpp"
#include "LuauBehaviourFixture.hpp"
#include "LuauPreHitTestConstants.hpp"

using rtype::engine::ScriptException;
using rtype::game::Intent;

/**
 * Given a body whose script declared a pre_hit function that returns false
 * When it is asked about a hit
 * Then the hit is cancelled
 */
TEST_F(LuauBehaviourFixture, APreHitThatReturnsFalseCancelsTheHit) {
  load(SHIELDED_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();
  static_cast<void>(steer(SHIELDED_BEHAVIOUR, body));

  const bool lets = body.behaviourState->letsHit(ANY_HIT);

  EXPECT_FALSE(lets);
}

/**
 * Given a body whose script declared a pre_hit function that returns nothing
 * When it is asked about a hit
 * Then the hit applies
 */
TEST_F(LuauBehaviourFixture, APreHitThatReturnsNothingLetsTheHitApply) {
  load(OPEN_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();
  static_cast<void>(steer(OPEN_BEHAVIOUR, body));

  const bool lets = body.behaviourState->letsHit(ANY_HIT);

  EXPECT_TRUE(lets);
}

/**
 * Given a body whose script declared no pre_hit function
 * When it is asked about a hit
 * Then the hit applies
 */
TEST_F(LuauBehaviourFixture, ABodyWithoutPreHitLetsEveryHit) {
  load(BARE_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();
  static_cast<void>(steer(BARE_BEHAVIOUR, body));

  const bool lets = body.behaviourState->letsHit(ANY_HIT);

  EXPECT_TRUE(lets);
}

/**
 * Given a body whose pre_hit function cancels only missile 42 lying 20 right
 * of its center and 20 above it
 * When it is asked about that hit
 * Then the hit is cancelled: the function read the missile and its offset
 */
TEST_F(LuauBehaviourFixture, APreHitReadsTheMissileAndWhereItLies) {
  load(WEAK_SPOT_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();
  static_cast<void>(steer(WEAK_SPOT_BEHAVIOUR, body));

  const bool lets = body.behaviourState->letsHit(HIT_ON_THE_WEAK_SPOT);

  EXPECT_FALSE(lets);
}

/**
 * Given a body whose pre_hit function fails
 * When it is asked about a hit
 * Then the hit applies
 */
TEST_F(LuauBehaviourFixture, AFailingPreHitLetsTheHitApply) {
  load(CRACKED_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();
  static_cast<void>(steer(CRACKED_BEHAVIOUR, body));

  const bool lets = body.behaviourState->letsHit(ANY_HIT);

  EXPECT_TRUE(lets);
}

/**
 * Given a body whose pre_hit function failed on a hit
 * When the body is steered at the next tick
 * Then it stays still and the failure is reported once, as a failed step
 */
TEST_F(LuauBehaviourFixture, AFailedPreHitStopsTheBodyAtItsNextStep) {
  load(CRACKED_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();
  static_cast<void>(steer(CRACKED_BEHAVIOUR, body));
  static_cast<void>(body.behaviourState->letsHit(ANY_HIT));

  beginTick();
  const Intent stopped = steer(CRACKED_BEHAVIOUR, body);

  EXPECT_EQ(stopped.direction.x, 0.0F);
  EXPECT_EQ(failures().messages.size(), REPORTED_ONCE);
}

/**
 * Given a body whose pre_hit function yields false
 * When it is asked about a hit
 * Then the hit applies: a pre_hit function may not wait
 */
TEST_F(LuauBehaviourFixture, APreHitThatYieldsLetsTheHitApply) {
  load(STALLING_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();
  static_cast<void>(steer(STALLING_BEHAVIOUR, body));

  const bool lets = body.behaviourState->letsHit(ANY_HIT);

  EXPECT_TRUE(lets);
}

/**
 * Given a body whose script cancelled every hit, then called
 * behaviours.pre_hit(nil) on its next step
 * When it is asked about a hit
 * Then the hit applies, the script having failed nowhere
 */
TEST_F(LuauBehaviourFixture, APreHitOfNilForgetsTheFunction) {
  load(DROPPING_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();
  static_cast<void>(steer(DROPPING_BEHAVIOUR, body));
  beginTick();
  static_cast<void>(steer(DROPPING_BEHAVIOUR, body));

  const bool lets = body.behaviourState->letsHit(ANY_HIT);

  EXPECT_TRUE(lets);
  EXPECT_TRUE(failures().messages.empty());
}

/**
 * Given a body whose script declared a pre_hit function that cancels every
 * hit, then failed in the same step
 * When it is asked about a hit
 * Then the hit applies: the pre_hit function stops with the script
 */
TEST_F(LuauBehaviourFixture, AStoppedScriptLetsEveryHit) {
  load(BRITTLE_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();
  static_cast<void>(steer(BRITTLE_BEHAVIOUR, body));

  const bool lets = body.behaviourState->letsHit(ANY_HIT);

  EXPECT_TRUE(lets);
}

/**
 * Given a script that calls behaviours.pre_hit while it is read
 * When it is run
 * Then it fails: only the function of a body may declare one
 */
TEST_F(LuauBehaviourFixture, RefusesAPreHitOutsideTheFunctionOfABody) {
  EXPECT_THROW(load(PRE_HIT_AT_READING_SCRIPT), ScriptException);
}

/**
 * Given a body whose pre_hit function cancels every hit, and steps that spent
 * the budget of the tick
 * When the body is asked about a hit in that tick
 * Then the hit applies without a call of its function
 */
TEST_F(LuauBehaviourFixture, APreHitPastTheBudgetOfTheTickLetsTheHitApply) {
  load(GUARDED_AND_STUCK_SCRIPT);
  SteeredBody guarded{.position = BODY_IN_THE_MIDDLE};
  beginTick();
  static_cast<void>(steer(GUARDED_BEHAVIOUR, guarded));
  beginTick();
  for (std::size_t step = 0; step < STEPS_THAT_SPEND_A_TICK; ++step) {
    SteeredBody stuck{.position = BODY_IN_THE_MIDDLE};
    static_cast<void>(steer(STUCK_BEHAVIOUR, stuck));
  }

  const bool lets = guarded.behaviourState->letsHit(ANY_HIT);

  EXPECT_TRUE(lets);
}

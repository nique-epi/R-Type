#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ActionKey.hpp"
#include "BehaviourScriptsTestConstants.hpp"
#include "Intent.hpp"
#include "LuauBehaviourFixture.hpp"
#include "LuauBehaviourTestConstants.hpp"
#include "RecordingBodyOrders.hpp"

using rtype::game::ActionKey;
using rtype::game::Intent;

/**
 * Given a body whose script waits 50 ms going left, then goes down
 * When it is steered for the ticks of that wait
 * Then it goes left on each of them
 */
TEST_F(LuauBehaviourFixture, HoldsTheIntentOfAWaitForEachTickOfIt) {
  load(PACER_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};

  for (std::size_t tick = 0; tick < TICKS_OF_THE_PACER_WAIT; ++tick) {
    beginTick();
    const Intent intent = steer(PACER_BEHAVIOUR, body);

    EXPECT_EQ(intent.direction.x, FULL_LEFTWARD);
    EXPECT_EQ(intent.direction.y, 0.0F);
  }
}

/**
 * Given a body whose script waits 50 ms going left, then goes down
 * When the wait is over
 * Then the script goes on after its wait, and the body goes down
 */
TEST_F(LuauBehaviourFixture, ResumesTheScriptOnTheTickItsWaitEnds) {
  load(PACER_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  for (std::size_t tick = 0; tick < TICKS_OF_THE_PACER_WAIT; ++tick) {
    beginTick();
    static_cast<void>(steer(PACER_BEHAVIOUR, body));
  }

  beginTick();
  const Intent intent = steer(PACER_BEHAVIOUR, body);

  EXPECT_EQ(intent.direction.x, 0.0F);
  EXPECT_EQ(intent.direction.y, FULL_DOWNWARD);
}

/**
 * Given a body whose script starts an attack and waits
 * When it is steered on the first tick of the wait
 * Then it starts that attack
 */
TEST_F(LuauBehaviourFixture, StartsTheActionOfAWaitOnItsFirstTick) {
  load(STRIKER_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();

  const Intent intent = steer(STRIKER_BEHAVIOUR, body);

  EXPECT_EQ(intent.startsAction.value_or(ActionKey{}).attack, STRIKER_ATTACK);
}

/**
 * Given a body whose script started an attack and waits
 * When it is steered on the next tick of the wait
 * Then it does not start the attack again
 */
TEST_F(LuauBehaviourFixture, StartsTheActionOfAWaitOnlyOnce) {
  load(STRIKER_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();
  static_cast<void>(steer(STRIKER_BEHAVIOUR, body));

  beginTick();
  const Intent intent = steer(STRIKER_BEHAVIOUR, body);

  EXPECT_FALSE(intent.startsAction.has_value());
}

/**
 * Given bodies whose scripts each hold a large table while they wait
 * When the bodies are destroyed during their wait
 * Then the memory of their scripts is released
 */
TEST_F(LuauBehaviourFixture, ReleasesTheMemoryOfABodyDestroyedDuringAWait) {
  load(HOARDER_SCRIPT);
  std::vector<SteeredBody> bodies(HOARDING_BODIES,
                                  SteeredBody{.position = BODY_IN_THE_MIDDLE});
  beginTick();
  for (SteeredBody& body : bodies) {
    static_cast<void>(steer(HOARDER_BEHAVIOUR, body));
  }
  const std::size_t whileWaiting = memoryAfterCollection();

  bodies.clear();

  EXPECT_LE(memoryAfterCollection() + BYTES_OF_THE_HOARDS, whileWaiting);
}

/**
 * Given two bodies of one behaviour whose script fails for one of them
 * When both are steered on the next tick
 * Then the failing body stays still and the other one goes on
 */
TEST_F(LuauBehaviourFixture, AFailingScriptStopsThatBodyOnly) {
  load(PICKY_SCRIPT);
  SteeredBody failing{.position = BODY_ON_THE_LEFT};
  SteeredBody healthy{.position = BODY_ON_THE_RIGHT};
  beginTick();
  static_cast<void>(steer(PICKY_BEHAVIOUR, failing));
  failing.position = BODY_ON_THE_RIGHT;

  beginTick();
  const Intent stopped = steer(PICKY_BEHAVIOUR, failing);
  const Intent going = steer(PICKY_BEHAVIOUR, healthy);

  EXPECT_EQ(stopped.direction.x, 0.0F);
  EXPECT_EQ(going.direction.x, FULL_LEFTWARD);
  EXPECT_EQ(failures().messages.size(), REPORTED_ONCE);
}

/**
 * Given a body of a game whose script fails
 * When it is steered
 * Then the game is asked to remove that body
 */
TEST_F(LuauBehaviourFixture, RemovesFromTheGameABodyWhoseScriptFails) {
  RecordingBodyOrders world{GIVEN_NETWORK_ID};
  attach(world);
  load(PICKY_SCRIPT);
  SteeredBody failing{.position = BODY_ON_THE_LEFT,
                      .networkId = FAILING_BODY_ID};
  beginTick();

  static_cast<void>(steer(PICKY_BEHAVIOUR, failing));

  ASSERT_EQ(world.removals().size(), 1U);
  EXPECT_EQ(world.removals().front(), FAILING_BODY_ID);
}

/**
 * Given a body whose script never ends its step
 * When it is steered, then another body of the same behaviour
 * Then the first is stopped by its step budget and the second goes on
 */
TEST_F(LuauBehaviourFixture, AStepPastItsBudgetStopsThatBodyOnly) {
  load(RUNAWAY_SCRIPT);
  SteeredBody runaway{.position = BODY_ON_THE_LEFT};
  SteeredBody healthy{.position = BODY_ON_THE_RIGHT};
  beginTick();

  const Intent stopped = steer(RUNAWAY_BEHAVIOUR, runaway);
  beginTick();
  const Intent going = steer(RUNAWAY_BEHAVIOUR, healthy);

  EXPECT_EQ(stopped.direction.x, 0.0F);
  EXPECT_EQ(going.direction.x, FULL_LEFTWARD);
  EXPECT_EQ(failures().messages.size(), REPORTED_ONCE);
}

/**
 * Given a script that yields without behaviours.wait
 * When its body is steered
 * Then the body stays still and the failure is reported
 */
TEST_F(LuauBehaviourFixture, RefusesAYieldOutsideAWait) {
  load(YIELDER_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();

  const Intent intent = steer(YIELDER_BEHAVIOUR, body);

  EXPECT_EQ(intent.direction.x, 0.0F);
  EXPECT_EQ(failures().messages.size(), REPORTED_ONCE);
}

/**
 * Given a script that creates a drifter at (10, 20)
 * When its body is steered in a game
 * Then the game is asked for that drifter at that place
 */
TEST_F(LuauBehaviourFixture, AsksTheGameForTheBodyAScriptCreates) {
  RecordingBodyOrders world{GIVEN_NETWORK_ID};
  attach(world);
  load(BUILDER_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();

  static_cast<void>(steer(BUILDER_BEHAVIOUR, body));

  ASSERT_EQ(world.spawns().size(), 1U);
  EXPECT_EQ(world.spawns().front().type, BUILT_TYPE);
  EXPECT_EQ(world.spawns().front().position.x, BUILT_AT.x);
  EXPECT_EQ(world.spawns().front().position.y, BUILT_AT.y);
}

/**
 * Given a script that removes the body it just created
 * When its body is steered in a game
 * Then the game is asked to remove the id it gave that body
 */
TEST_F(LuauBehaviourFixture, RemovesTheBodyNamedByTheIdSpawnReturned) {
  RecordingBodyOrders world{GIVEN_NETWORK_ID};
  attach(world);
  load(BUILDER_SCRIPT);
  SteeredBody body{.position = BODY_IN_THE_MIDDLE};
  beginTick();

  static_cast<void>(steer(BUILDER_BEHAVIOUR, body));

  ASSERT_EQ(world.removals().size(), 1U);
  EXPECT_EQ(world.removals().front(), GIVEN_NETWORK_ID);
}

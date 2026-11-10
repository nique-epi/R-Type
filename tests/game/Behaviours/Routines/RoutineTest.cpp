#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include "Intent.hpp"
#include "Routine.hpp"
#include "RoutineTestConstants.hpp"

using rtype::game::Intent;
using rtype::game::Routine;

/**
 * Given a routine that has not stepped yet
 * When it is asked what its body holds
 * Then nothing is held: its first step is due
 */
TEST(Routine, AsksForItsFirstStepAtOnce) {
  Routine routine;

  EXPECT_FALSE(routine.heldThisTick().has_value());
}

/**
 * Given a step going left followed by a wait of three ticks
 * When the two ticks after the step pass
 * Then the body goes left on each of them
 */
TEST(Routine, HoldsTheIntentOfAStepForEachTickOfItsWait) {
  Routine routine;
  static_cast<void>(routine.stepped(GOING_LEFT, THREE_TICKS));

  for (std::uint32_t tick = 0; tick < TICKS_HELD_AFTER_THE_STEP; ++tick) {
    const std::optional<Intent> held = routine.heldThisTick();

    ASSERT_TRUE(held.has_value());
    EXPECT_EQ(held.value_or(Intent{}).direction.x, GOING_LEFT.direction.x);
  }
}

/**
 * Given a step followed by a wait of three ticks
 * When the three ticks of the wait have passed
 * Then the next step is due
 */
TEST(Routine, AsksForTheNextStepOnceTheWaitIsOver) {
  Routine routine;
  static_cast<void>(routine.stepped(GOING_LEFT, THREE_TICKS));
  for (std::uint32_t tick = 0; tick < TICKS_HELD_AFTER_THE_STEP; ++tick) {
    static_cast<void>(routine.heldThisTick());
  }

  EXPECT_FALSE(routine.heldThisTick().has_value());
}

/**
 * Given a step without a wait
 * When the next tick comes
 * Then the next step is due
 */
TEST(Routine, AsksForAStepEveryTickWithoutAWait) {
  Routine routine;
  static_cast<void>(routine.stepped(GOING_LEFT, NO_WAIT));

  EXPECT_FALSE(routine.heldThisTick().has_value());
}

/**
 * Given a step that starts an attack, then waits
 * When the step is recorded
 * Then the body starts the attack on that tick
 */
TEST(Routine, StartsTheActionOfAStepOnItsTick) {
  Routine routine;

  const Intent intent = routine.stepped(STRIKING, THREE_TICKS);

  EXPECT_TRUE(intent.startsAction.has_value());
}

/**
 * Given a step that started an attack, then waits
 * When the next tick of the wait comes
 * Then the body does not start the attack again
 */
TEST(Routine, StartsTheActionOfAStepOnlyOnce) {
  Routine routine;
  static_cast<void>(routine.stepped(STRIKING, THREE_TICKS));

  const std::optional<Intent> held = routine.heldThisTick();

  ASSERT_TRUE(held.has_value());
  EXPECT_FALSE(held.value_or(STRIKING).startsAction.has_value());
}

/**
 * Given a routine stopped during a wait going left
 * When the next tick comes
 * Then the body stands still
 */
TEST(Routine, StandsStillOnceStopped) {
  Routine routine;
  static_cast<void>(routine.stepped(GOING_LEFT, THREE_TICKS));

  routine.stop();

  EXPECT_EQ(routine.heldThisTick().value_or(GOING_LEFT).direction.x, 0.0F);
}

#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <string>
#include "HeldIntent.hpp"
#include "HeldIntentTestConstants.hpp"
#include "Intent.hpp"

using rtype::game::HeldIntent;
using rtype::game::Intent;

/**
 * Given a held intent that never held anything
 * When it is asked what its body holds
 * Then nothing is held: its first step is due
 */
TEST(HeldIntent, AsksForItsFirstStepAtOnce) {
  HeldIntent heldIntent;

  EXPECT_FALSE(heldIntent.heldThisTick().has_value());
}

/**
 * Given a step going left followed by a wait of three ticks
 * When the two ticks after the step pass
 * Then the body goes left on each of them
 */
TEST(HeldIntent, HoldsTheIntentOfAStepForEachTickOfItsWait) {
  HeldIntent heldIntent;
  static_cast<void>(heldIntent.startHolding(GOING_LEFT, THREE_TICKS));

  for (std::uint32_t tick = 0; tick < TICKS_HELD_AFTER_THE_STEP; ++tick) {
    const std::optional<Intent> held = heldIntent.heldThisTick();

    ASSERT_TRUE(held.has_value());
    EXPECT_EQ(held.value_or(Intent{}).direction.x, GOING_LEFT.direction.x);
  }
}

/**
 * Given a step followed by a wait of three ticks
 * When the three ticks of the wait have passed
 * Then the next step is due
 */
TEST(HeldIntent, AsksForTheNextStepOnceTheWaitIsOver) {
  HeldIntent heldIntent;
  static_cast<void>(heldIntent.startHolding(GOING_LEFT, THREE_TICKS));
  for (std::uint32_t tick = 0; tick < TICKS_HELD_AFTER_THE_STEP; ++tick) {
    static_cast<void>(heldIntent.heldThisTick());
  }

  EXPECT_FALSE(heldIntent.heldThisTick().has_value());
}

/**
 * Given a step without a wait
 * When the next tick comes
 * Then the next step is due
 */
TEST(HeldIntent, AsksForAStepEveryTickWithoutAWait) {
  HeldIntent heldIntent;
  static_cast<void>(heldIntent.startHolding(GOING_LEFT, NO_WAIT));

  EXPECT_FALSE(heldIntent.heldThisTick().has_value());
}

/**
 * Given a step that starts an attack, then waits
 * When the step is recorded
 * Then the body starts the attack on that tick
 */
TEST(HeldIntent, StartsTheActionOfAStepOnItsTick) {
  HeldIntent heldIntent;

  const Intent intent = heldIntent.startHolding(STRIKING, THREE_TICKS);

  EXPECT_TRUE(intent.startsAction.has_value());
}

/**
 * Given a step that started an attack, then waits
 * When the next tick of the wait comes
 * Then the body does not start the attack again
 */
TEST(HeldIntent, StartsTheActionOfAStepOnlyOnce) {
  HeldIntent heldIntent;
  static_cast<void>(heldIntent.startHolding(STRIKING, THREE_TICKS));

  const std::optional<Intent> held = heldIntent.heldThisTick();

  ASSERT_TRUE(held.has_value());
  EXPECT_FALSE(held.value_or(STRIKING).startsAction.has_value());
}

/**
 * Given a step that played an effect, then waits
 * When the next tick of the wait comes
 * Then the body does not play the effect again
 */
TEST(HeldIntent, PlaysTheEffectOfAStepOnlyOnce) {
  HeldIntent heldIntent;
  const Intent sparkling{.steered = true,
                         .playsEffect = std::string{SPARK_EFFECT}};
  static_cast<void>(heldIntent.startHolding(sparkling, THREE_TICKS));

  const std::optional<Intent> held = heldIntent.heldThisTick();

  ASSERT_TRUE(held.has_value());
  EXPECT_FALSE(held.value_or(sparkling).playsEffect.has_value());
}

/**
 * Given a held intent stopped during a wait going left
 * When the next tick comes
 * Then the body stands still
 */
TEST(HeldIntent, StandsStillOnceStopped) {
  HeldIntent heldIntent;
  static_cast<void>(heldIntent.startHolding(GOING_LEFT, THREE_TICKS));

  heldIntent.stop();

  EXPECT_EQ(heldIntent.heldThisTick().value_or(GOING_LEFT).direction.x, 0.0F);
}

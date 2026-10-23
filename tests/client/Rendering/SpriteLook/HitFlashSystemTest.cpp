#include <gtest/gtest.h>
#include "Entity.hpp"
#include "FlashedScene.hpp"
#include "HitFlashSystemTestConstants.hpp"
#include "ScreenTestConstants.hpp"

using rtype::engine::Entity;

/**
 * Given an entity flashed for 60 ms
 * When the system plays one nanosecond less than 60 ms
 * Then the entity is still flashed
 */
TEST(HitFlashSystem, AFlashLastsUntilItsTimeIsSpent) {
  FlashedScene scene;
  const Entity hit = scene.flashed(FLASH_TIME);

  scene.play(FLASH_TIME - ONE_TICK);

  EXPECT_TRUE(scene.isFlashed(hit));
}

/**
 * Given an entity flashed for 60 ms
 * When the system plays 60 ms
 * Then the entity is no longer flashed
 */
TEST(HitFlashSystem, AFlashEndsOnceItsTimeIsSpent) {
  FlashedScene scene;
  const Entity hit = scene.flashed(FLASH_TIME);

  scene.play(FLASH_TIME);

  EXPECT_FALSE(scene.isFlashed(hit));
}

/**
 * Given an entity flashed for 60 ms
 * When its flash ends
 * Then the entity itself stays alive
 */
TEST(HitFlashSystem, AnEntityWhoseFlashEndedStaysAlive) {
  FlashedScene scene;
  const Entity hit = scene.flashed(FLASH_TIME);

  scene.play(FLASH_TIME);

  EXPECT_TRUE(scene.isAlive(hit));
}

/**
 * Given one entity flashed for 60 ms and another for 120 ms
 * When the system plays 60 ms
 * Then the first is no longer flashed and the second still is
 */
TEST(HitFlashSystem, EachFlashEndsAtItsOwnTime) {
  FlashedScene scene;
  const Entity shortFlash = scene.flashed(FLASH_TIME);
  const Entity longFlash = scene.flashed(LONGER_FLASH_TIME);

  scene.play(FLASH_TIME);

  EXPECT_FALSE(scene.isFlashed(shortFlash));
  EXPECT_TRUE(scene.isFlashed(longFlash));
}

/**
 * Given no flashed entity
 * When the system plays a step
 * Then nothing fails
 */
TEST(HitFlashSystem, AStepWithoutFlashChangesNothing) {
  FlashedScene scene;

  EXPECT_NO_THROW(scene.play(FLASH_TIME));
}

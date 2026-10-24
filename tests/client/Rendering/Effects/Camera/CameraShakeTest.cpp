#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include "CameraShake.hpp"
#include "CameraShakeFixtures.hpp"
#include "CameraShakeTestConstants.hpp"

using rtype::client::CameraShake;

/**
 * Given a camera without trauma
 * When a second passes
 * Then it never leaves its place
 */
TEST(CameraShake, NoTraumaNoShake) { EXPECT_FALSE(shakeKeeping(0.0F).moved); }

/**
 * Given the most trauma, kept for a second
 * When the offset is read every frame
 * Then the camera moves, never more than 6 pixels on an axis
 */
TEST(CameraShake, AFullTraumaShakesSixPixelsAtMost) {
  const ShakeOverASecond shake = shakeKeeping(FULL_TRAUMA);

  EXPECT_TRUE(shake.moved);
  EXPECT_LE(shake.largest, LARGEST_SHAKE);
}

/**
 * Given the trauma of a big catch, kept for a second
 * When the offset is read every frame
 * Then the camera is never more than 1 pixel off
 */
TEST(CameraShake, ABigCatchShakesOnePixelAtMost) {
  EXPECT_LE(shakeKeeping(BIG_CATCH_TRAUMA_REQUIRED).largest, 1);
}

/**
 * Given the trauma of a ship destroyed, 0.3
 * When 150 ms pass
 * Then some trauma is left: it loses 1.5 a second
 */
TEST(CameraShake, TraumaLastsUntilItDecays) {
  CameraShake shake;
  shake.addTrauma(SHIP_DOWN_TRAUMA_REQUIRED);

  shake.advance(BEFORE_A_TRAUMA_OF_03_IS_GONE);

  EXPECT_GT(shake.trauma(), 0.0F);
}

/**
 * Given the trauma of a ship destroyed, 0.3
 * When 250 ms pass
 * Then none is left and the camera is back in place
 */
TEST(CameraShake, TraumaIsGoneOnceItDecayed) {
  CameraShake shake;
  shake.addTrauma(SHIP_DOWN_TRAUMA_REQUIRED);

  shake.advance(AFTER_A_TRAUMA_OF_03_IS_GONE);

  EXPECT_EQ(shake.trauma(), 0.0F);
  EXPECT_EQ(shake.offset(), sf::Vector2i());
}

/**
 * Given a trauma of 0.8
 * When 0.8 more is added
 * Then the trauma is 1, the most
 */
TEST(CameraShake, TraumaIsCappedAtOne) {
  CameraShake shake;
  shake.addTrauma(HIGH_TRAUMA);

  shake.addTrauma(HIGH_TRAUMA);

  EXPECT_EQ(shake.trauma(), FULL_TRAUMA);
}

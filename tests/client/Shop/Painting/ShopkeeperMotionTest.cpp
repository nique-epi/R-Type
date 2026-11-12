#include <gtest/gtest.h>
#include "ShopkeeperEye.hpp"
#include "ShopkeeperMotion.hpp"
#include "ShopkeeperMotionTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::ShopkeeperEye;
using rtype::client::shopkeeperPoseAt;
using rtype::client::testing::A_QUARTER_OF_A_BREATH;
using rtype::client::testing::A_WHOLE_BREATH;
using rtype::client::testing::A_WHOLE_EYE_CYCLE;
using rtype::client::testing::HIGHEST_DROP;
using rtype::client::testing::LOWEST_DROP;
using rtype::client::testing::THE_FIRST_BLINK;
using rtype::client::testing::THE_GLANCE_LEFT;
using rtype::client::testing::THE_GLANCE_RIGHT;
using rtype::client::testing::THREE_QUARTERS_OF_A_BREATH;
using rtype::engine::Duration;

/**
 * Given a shop just opened
 * When the pose of the shopkeeper is asked
 * Then he is at his place of rest, looking ahead
 */
TEST(ShopkeeperMotion, StartsAtRestLookingAhead) {
  EXPECT_EQ(shopkeeperPoseAt(Duration::zero()).drop, 0);
  EXPECT_EQ(shopkeeperPoseAt(Duration::zero()).eye, ShopkeeperEye::Ahead);
}

/**
 * Given a shop shown for a quarter of a breath
 * When the pose of the shopkeeper is asked
 * Then he is 2 pixels under his place of rest
 */
TEST(ShopkeeperMotion, SinksAsHeBreathesOut) {
  EXPECT_EQ(shopkeeperPoseAt(A_QUARTER_OF_A_BREATH).drop, LOWEST_DROP);
}

/**
 * Given a shop shown for three quarters of a breath
 * When the pose of the shopkeeper is asked
 * Then he is 2 pixels above his place of rest
 */
TEST(ShopkeeperMotion, RisesAsHeBreathesIn) {
  EXPECT_EQ(shopkeeperPoseAt(THREE_QUARTERS_OF_A_BREATH).drop, HIGHEST_DROP);
}

/**
 * Given a shop shown for a whole breath
 * When the pose of the shopkeeper is asked
 * Then he is back at his place of rest
 */
TEST(ShopkeeperMotion, AWholeBreathBringsHimBackToRest) {
  EXPECT_EQ(shopkeeperPoseAt(A_WHOLE_BREATH).drop, 0);
}

/**
 * Given the moment of the first blink
 * When the pose of the shopkeeper is asked
 * Then his eye is closed
 */
TEST(ShopkeeperMotion, BlinksOnceInAWhile) {
  EXPECT_EQ(shopkeeperPoseAt(THE_FIRST_BLINK).eye, ShopkeeperEye::Closed);
}

/**
 * Given the moments of his two glances
 * When the pose of the shopkeeper is asked
 * Then his eye looks to his left, then to his right
 */
TEST(ShopkeeperMotion, GlancesLeftThenRight) {
  EXPECT_EQ(shopkeeperPoseAt(THE_GLANCE_LEFT).eye, ShopkeeperEye::Left);
  EXPECT_EQ(shopkeeperPoseAt(THE_GLANCE_RIGHT).eye, ShopkeeperEye::Right);
}

/**
 * Given a shop shown for a whole cycle of the eye, then for one blink more
 * When the pose of the shopkeeper is asked
 * Then the cycle started over: ahead, then closed at the same moment
 */
TEST(ShopkeeperMotion, TheEyeStartsItsCycleOver) {
  EXPECT_EQ(shopkeeperPoseAt(A_WHOLE_EYE_CYCLE).eye, ShopkeeperEye::Ahead);
  EXPECT_EQ(shopkeeperPoseAt(A_WHOLE_EYE_CYCLE + THE_FIRST_BLINK).eye,
            ShopkeeperEye::Closed);
}

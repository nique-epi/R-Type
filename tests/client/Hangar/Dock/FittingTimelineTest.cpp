#include <gtest/gtest.h>
#include "ArmPose.hpp"
#include "FittingTimeline.hpp"
#include "FittingTimelineTestConstants.hpp"

using rtype::client::ArmPose;
using rtype::client::fittingPoseAt;
using rtype::client::lookShownAt;
using rtype::client::testing::A_ROUTE;
using rtype::client::testing::BETWEEN_HATS;
using rtype::client::testing::END;
using rtype::client::testing::HALFWAY_BACK;
using rtype::client::testing::HULL_GRIP;
using rtype::client::testing::JUST_BEFORE_PLACING;
using rtype::client::testing::MOUNT_POINT;
using rtype::client::testing::PLACING;
using rtype::client::testing::START;
using rtype::client::testing::TOP_HAT_OVER_PARTY_HAT;
using rtype::client::testing::WEARS_NOTHING;
using rtype::client::testing::WEARS_PARTY_HAT;
using rtype::client::testing::WEARS_TOP_HAT;

/**
 * Given a fitting that starts
 * When the arm is asked where it is
 * Then its jaws are closed on the hull and carry nothing
 */
TEST(FittingTimeline, AtTheStartTheJawsAreClosedOnTheHull) {
  const ArmPose pose = fittingPoseAt(START, A_ROUTE);

  EXPECT_EQ(pose.target, HULL_GRIP);
  EXPECT_FLOAT_EQ(pose.open, 0.0F);
  EXPECT_FALSE(pose.carrying);
}

/**
 * Given a fitting halfway, the arm coming back from off screen
 * When the arm is asked what it holds
 * Then it carries the item
 */
TEST(FittingTimeline, HalfwayTheArmCarriesTheItem) {
  EXPECT_TRUE(fittingPoseAt(HALFWAY_BACK, A_ROUTE).carrying);
}

/**
 * Given a fitting just before the item touches the ship
 * When the arm is asked what it holds
 * Then it still carries the item
 */
TEST(FittingTimeline, JustBeforePlacingTheArmStillCarries) {
  EXPECT_TRUE(fittingPoseAt(JUST_BEFORE_PLACING, A_ROUTE).carrying);
}

/**
 * Given a fitting at the moment the item touches the ship
 * When the arm is asked where it is
 * Then its jaws are on the mount and hold nothing any more
 */
TEST(FittingTimeline, WhenPlacingTheJawsAreOnTheMountAndLetGo) {
  const ArmPose pose = fittingPoseAt(PLACING, A_ROUTE);

  EXPECT_EQ(pose.target, MOUNT_POINT);
  EXPECT_FALSE(pose.carrying);
}

/**
 * Given a fitting that ends
 * When the arm is asked where it is
 * Then its jaws are closed on the hull again
 */
TEST(FittingTimeline, AtTheEndTheJawsCloseOnTheHull) {
  const ArmPose pose = fittingPoseAt(END, A_ROUTE);

  EXPECT_EQ(pose.target, HULL_GRIP);
  EXPECT_FLOAT_EQ(pose.open, 0.0F);
}

/**
 * Given the top hat fitted over the party hat, halfway
 * When the look of the ship is asked
 * Then it still wears the party hat
 */
TEST(FittingTimeline, TheOldHatStaysWhileTheArmIsAway) {
  EXPECT_EQ(lookShownAt(HALFWAY_BACK, TOP_HAT_OVER_PARTY_HAT).head,
            WEARS_PARTY_HAT);
}

/**
 * Given the top hat fitted over the party hat, the arm above the mount
 * When the look of the ship is asked
 * Then the head wears nothing: the old hat is gone, the new one not set
 */
TEST(FittingTimeline, TheOldHatGoesBeforeTheNewOneIsSet) {
  EXPECT_EQ(lookShownAt(BETWEEN_HATS, TOP_HAT_OVER_PARTY_HAT).head,
            WEARS_NOTHING);
}

/**
 * Given the top hat fitted over the party hat, set down
 * When the look of the ship is asked
 * Then it wears the top hat
 */
TEST(FittingTimeline, TheNewHatIsOnOnceSetDown) {
  EXPECT_EQ(lookShownAt(PLACING, TOP_HAT_OVER_PARTY_HAT).head, WEARS_TOP_HAT);
}

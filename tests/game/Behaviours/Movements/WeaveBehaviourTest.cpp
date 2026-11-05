#include <gtest/gtest.h>
#include "BehaviourTestConstants.hpp"
#include "BehaviourView.hpp"
#include "DirectionConstants.hpp"
#include "Intent.hpp"
#include "WeaveBehaviour.hpp"

using rtype::game::BehaviourView;
using rtype::game::FULL_STEERING;
using rtype::game::Intent;
using rtype::game::WeaveBehaviour;
using rtype::game::testing::BODY_TOP_SPEED;
using rtype::game::testing::QUARTER_PERIOD;
using rtype::game::testing::QUARTER_PERIOD_VERTICAL_SHARE;
using rtype::game::testing::ROW_START;
using rtype::game::testing::SHARE_TOLERANCE;

/**
 * Given a body on its row at 240 units per second, whose next step of half a
 * second ends a quarter period after it appeared
 * When the weave behaviour steers it
 * Then it asks for the share that lands 48 units below the row, the bottom of
 * the weave
 */
TEST(WeaveBehaviour, SteersToTheBottomOfItsWeaveAtAQuarterPeriod) {
  const WeaveBehaviour weave;

  const Intent intent = weave.steer(BehaviourView{.position = ROW_START,
                                                  .origin = ROW_START,
                                                  .age = QUARTER_PERIOD,
                                                  .elapsed = QUARTER_PERIOD,
                                                  .topSpeed = BODY_TOP_SPEED});

  EXPECT_NEAR(intent.direction.y, QUARTER_PERIOD_VERTICAL_SHARE,
              SHARE_TOLERANCE);
}

/**
 * Given a body on its row
 * When the weave behaviour steers it
 * Then it asks for the full speed to the left, and wants to shoot
 */
TEST(WeaveBehaviour, CrossesLeftAtFullSpeedAndShoots) {
  const WeaveBehaviour weave;

  const Intent intent = weave.steer(BehaviourView{.position = ROW_START,
                                                  .origin = ROW_START,
                                                  .age = QUARTER_PERIOD,
                                                  .elapsed = QUARTER_PERIOD,
                                                  .topSpeed = BODY_TOP_SPEED});

  EXPECT_EQ(intent.direction.x, -FULL_STEERING);
  EXPECT_TRUE(intent.shooting);
}

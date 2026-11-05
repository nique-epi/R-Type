#include <gtest/gtest.h>
#include "BehaviourTestConstants.hpp"
#include "BehaviourView.hpp"
#include "DirectionConstants.hpp"
#include "Intent.hpp"
#include "StraightBehaviour.hpp"

using rtype::game::BehaviourView;
using rtype::game::FULL_STEERING;
using rtype::game::Intent;
using rtype::game::StraightBehaviour;
using rtype::game::testing::BODY_TOP_SPEED;
using rtype::game::testing::QUARTER_PERIOD;
using rtype::game::testing::ROW_START;

/**
 * Given a body a quarter period after it appeared on its row
 * When the straight behaviour steers it
 * Then it asks for the full speed to the left and nothing vertically
 */
TEST(StraightBehaviour, CrossesLeftOnItsRowAtFullSpeed) {
  const StraightBehaviour straight;

  const Intent intent =
      straight.steer(BehaviourView{.position = ROW_START,
                                   .origin = ROW_START,
                                   .age = QUARTER_PERIOD,
                                   .elapsed = QUARTER_PERIOD,
                                   .topSpeed = BODY_TOP_SPEED});

  EXPECT_EQ(intent.direction.x, -FULL_STEERING);
  EXPECT_EQ(intent.direction.y, 0.0F);
}

/**
 * Given a body that just appeared
 * When the straight behaviour steers it
 * Then it wants to shoot: how often it fires is the interval of its Gun
 */
TEST(StraightBehaviour, WantsToShoot) {
  const StraightBehaviour straight;

  const Intent intent =
      straight.steer(BehaviourView{.position = ROW_START,
                                   .origin = ROW_START,
                                   .age = QUARTER_PERIOD,
                                   .elapsed = QUARTER_PERIOD,
                                   .topSpeed = BODY_TOP_SPEED});

  EXPECT_TRUE(intent.shooting);
}

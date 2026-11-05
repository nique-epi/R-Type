#include <gtest/gtest.h>
#include "BehaviourView.hpp"
#include "DirectionConstants.hpp"
#include "DrillbackBehaviour.hpp"
#include "DrillbackBehaviourFixtures.hpp"
#include "DrillbackBehaviourTestConstants.hpp"
#include "Intent.hpp"

using rtype::game::BehaviourView;
using rtype::game::DrillbackBehaviour;
using rtype::game::FULL_STEERING;
using rtype::game::Intent;
using rtype::game::testing::CHARGE;
using rtype::game::testing::CHARGE_TIME;
using rtype::game::testing::DRIFT_SHARE;
using rtype::game::testing::drillbackView;
using rtype::game::testing::IN_PLAY;
using rtype::game::testing::NOT_IN_YET;
using rtype::game::testing::SHAKING;
using rtype::game::testing::STUN;

/**
 * Given a rested Drillback not yet left of where it may charge
 * When it steers
 * Then it drifts with the sky and starts nothing
 */
TEST(DrillbackBehaviour, BeforeItIsInItDriftsWithTheSky) {
  const Intent intent = DrillbackBehaviour{}.steer(drillbackView(NOT_IN_YET));

  EXPECT_FLOAT_EQ(intent.direction.x, DRIFT_SHARE);
  EXPECT_FALSE(intent.startsAction.has_value());
}

/**
 * Given a rested Drillback in play
 * When it steers
 * Then it starts shaking, still drifting
 */
TEST(DrillbackBehaviour, InPlayAndRestedItStartsShaking) {
  const Intent intent = DrillbackBehaviour{}.steer(drillbackView(IN_PLAY));

  EXPECT_EQ(intent.startsAction, SHAKING);
  EXPECT_FLOAT_EQ(intent.direction.x, DRIFT_SHARE);
}

/**
 * Given a Drillback at the start of its charge
 * When it steers
 * Then it asks for its full speed to the left
 */
TEST(DrillbackBehaviour, ItChargesAtItsFullSpeed) {
  BehaviourView view = drillbackView(IN_PLAY);
  view.action = CHARGE;
  view.actionAge = {};

  const Intent intent = DrillbackBehaviour{}.steer(view);

  EXPECT_EQ(intent.direction.x, -FULL_STEERING);
  EXPECT_EQ(intent.direction.y, 0.0F);
}

/**
 * Given a Drillback whose charge has lasted its 700 ms
 * When it steers
 * Then its stun starts
 */
TEST(DrillbackBehaviour, ItsChargeEndsInItsStun) {
  BehaviourView view = drillbackView(IN_PLAY);
  view.action = CHARGE;
  view.actionAge = CHARGE_TIME;

  EXPECT_EQ(DrillbackBehaviour{}.steer(view).startsAction, STUN);
}

/**
 * Given a stunned Drillback
 * When it steers
 * Then it drifts with the sky again
 */
TEST(DrillbackBehaviour, StunnedItDriftsAgain) {
  BehaviourView view = drillbackView(IN_PLAY);
  view.action = STUN;
  view.actionAge = {};

  EXPECT_FLOAT_EQ(DrillbackBehaviour{}.steer(view).direction.x, DRIFT_SHARE);
}

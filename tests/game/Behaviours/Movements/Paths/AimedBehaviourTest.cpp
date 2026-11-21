#include <gtest/gtest.h>
#include <memory>
#include "AimedBehaviour.hpp"
#include "AimedBehaviourTestConstants.hpp"
#include "BehaviourView.hpp"
#include "DirectionConstants.hpp"
#include "IBehaviourState.hpp"
#include "Intent.hpp"

using rtype::game::AimedBehaviour;
using rtype::game::BehaviourView;
using rtype::game::FULL_STEERING;
using rtype::game::IBehaviourState;
using rtype::game::Intent;
using rtype::game::testing::AIMED_ACROSS;
using rtype::game::testing::AIMED_DOWN;
using rtype::game::testing::SHIP_BEHIND_THE_SHOT;
using rtype::game::testing::SHIP_DOWN_LEFT_OF_THE_SHOT;
using rtype::game::testing::SHIP_UP_LEFT_OF_THE_SHOT;
using rtype::game::testing::WHERE_THE_SHOT_STARTS;

/**
 * Given a shot on its first tick and a ship 300 units left of it, 400 below
 * When the aimed behaviour steers it
 * Then it heads straight at the ship: 0.6 of its speed left, 0.8 down
 */
TEST(AimedBehaviour, HeadsAtTheShipLeftOfIt) {
  std::shared_ptr<IBehaviourState> kept;

  const Intent intent = AimedBehaviour{}.steer(
      BehaviourView{.position = WHERE_THE_SHOT_STARTS,
                    .settings = {},
                    .nearestShip = SHIP_DOWN_LEFT_OF_THE_SHOT,
                    .behaviourState = &kept});

  EXPECT_FLOAT_EQ(intent.direction.x, AIMED_ACROSS);
  EXPECT_FLOAT_EQ(intent.direction.y, AIMED_DOWN);
}

/**
 * Given a shot aimed on its first tick at a ship below and left of it
 * When the ship flies up before the next tick, and the behaviour steers again
 * Then the shot keeps the heading of its first tick: it never follows the ship
 */
TEST(AimedBehaviour, KeepsTheHeadingOfItsFirstTick) {
  std::shared_ptr<IBehaviourState> kept;
  const AimedBehaviour aimed;
  static_cast<void>(
      aimed.steer(BehaviourView{.position = WHERE_THE_SHOT_STARTS,
                                .settings = {},
                                .nearestShip = SHIP_DOWN_LEFT_OF_THE_SHOT,
                                .behaviourState = &kept}));

  const Intent intent =
      aimed.steer(BehaviourView{.position = WHERE_THE_SHOT_STARTS,
                                .settings = {},
                                .nearestShip = SHIP_UP_LEFT_OF_THE_SHOT,
                                .behaviourState = &kept});

  EXPECT_FLOAT_EQ(intent.direction.x, AIMED_ACROSS);
  EXPECT_FLOAT_EQ(intent.direction.y, AIMED_DOWN);
}

/**
 * Given a shot on its first tick and no ship in play
 * When the aimed behaviour steers it
 * Then it flies left at its full speed, the way of the missiles of an enemy
 */
TEST(AimedBehaviour, FliesLeftWithNoShipInPlay) {
  std::shared_ptr<IBehaviourState> kept;

  const Intent intent =
      AimedBehaviour{}.steer(BehaviourView{.position = WHERE_THE_SHOT_STARTS,
                                           .settings = {},
                                           .behaviourState = &kept});

  EXPECT_FLOAT_EQ(intent.direction.x, -FULL_STEERING);
  EXPECT_FLOAT_EQ(intent.direction.y, 0.0F);
}

/**
 * Given a shot on its first tick and the nearest ship right of it, behind it
 * When the aimed behaviour steers it
 * Then it flies left: an enemy shot never turns back towards its shooter
 */
TEST(AimedBehaviour, FliesLeftWhenTheShipIsBehindIt) {
  std::shared_ptr<IBehaviourState> kept;

  const Intent intent =
      AimedBehaviour{}.steer(BehaviourView{.position = WHERE_THE_SHOT_STARTS,
                                           .settings = {},
                                           .nearestShip = SHIP_BEHIND_THE_SHOT,
                                           .behaviourState = &kept});

  EXPECT_FLOAT_EQ(intent.direction.x, -FULL_STEERING);
  EXPECT_FLOAT_EQ(intent.direction.y, 0.0F);
}

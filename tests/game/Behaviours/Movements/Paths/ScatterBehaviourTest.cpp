#include <gtest/gtest.h>
#include "BehaviourTestConstants.hpp"
#include "BehaviourView.hpp"
#include "DirectionConstants.hpp"
#include "Intent.hpp"
#include "ScatterBehaviour.hpp"
#include "ScatterBehaviourTestConstants.hpp"

using rtype::game::BehaviourView;
using rtype::game::FULL_STEERING;
using rtype::game::Intent;
using rtype::game::ScatterBehaviour;
using rtype::game::testing::BODY_TOP_SPEED;
using rtype::game::testing::BURST_CENTER;
using rtype::game::testing::DIAGONAL_SHARE;
using rtype::game::testing::QUARTER_PERIOD;
using rtype::game::testing::SCATTER_SHARE_TOLERANCE;
using rtype::game::testing::UP_RIGHT_OF_THE_CENTER;

/**
 * Given a shard 8 units up and to the right of the center it steers from
 * When the scatter behaviour steers it
 * Then it asks for the full speed on that diagonal: right and up, equally
 */
TEST(ScatterBehaviour, FliesStraightAwayFromItsOrigin) {
  const ScatterBehaviour scatter;

  const Intent intent =
      scatter.steer(BehaviourView{.position = UP_RIGHT_OF_THE_CENTER,
                                  .origin = BURST_CENTER,
                                  .age = QUARTER_PERIOD,
                                  .elapsed = QUARTER_PERIOD,
                                  .topSpeed = BODY_TOP_SPEED,
                                  .settings = {}});

  EXPECT_NEAR(intent.direction.x, DIAGONAL_SHARE, SCATTER_SHARE_TOLERANCE);
  EXPECT_NEAR(intent.direction.y, -DIAGONAL_SHARE, SCATTER_SHARE_TOLERANCE);
}

/**
 * Given a shard still on the point it steers from
 * When the scatter behaviour steers it
 * Then it flies left at its full speed, the way of the missiles of an enemy
 */
TEST(ScatterBehaviour, AShardOnItsOriginFliesLeft) {
  const ScatterBehaviour scatter;

  const Intent intent = scatter.steer(BehaviourView{.position = BURST_CENTER,
                                                    .origin = BURST_CENTER,
                                                    .age = QUARTER_PERIOD,
                                                    .elapsed = QUARTER_PERIOD,
                                                    .topSpeed = BODY_TOP_SPEED,
                                                    .settings = {}});

  EXPECT_EQ(intent.direction.x, -FULL_STEERING);
  EXPECT_EQ(intent.direction.y, 0.0F);
}

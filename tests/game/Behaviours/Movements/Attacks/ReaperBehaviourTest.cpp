#include <gtest/gtest.h>
#include <optional>
#include "BehaviourView.hpp"
#include "DirectionConstants.hpp"
#include "Intent.hpp"
#include "ReaperBehaviour.hpp"
#include "ReaperBehaviourFixtures.hpp"
#include "ReaperBehaviourTestConstants.hpp"

using rtype::game::BehaviourView;
using rtype::game::FULL_STEERING;
using rtype::game::Intent;
using rtype::game::ReaperBehaviour;
using rtype::game::testing::FAR_RIGHT_OF_THE_POINT;
using rtype::game::testing::HUNTED_SHIP;
using rtype::game::testing::NOT_RESTED;
using rtype::game::testing::reaperView;
using rtype::game::testing::SCYTHE_RECOVER;
using rtype::game::testing::SCYTHE_SIGNAL;
using rtype::game::testing::SCYTHE_STRIKE;
using rtype::game::testing::SCYTHE_STRIKE_TIME;
using rtype::game::testing::SHARE_TO_THE_POINT;
using rtype::game::testing::STRIKING_POINT;

/**
 * Given an idle Reaper with no ship in play
 * When it steers
 * Then it crosses the playfield to the left at its full speed
 */
TEST(ReaperBehaviour, WithoutAShipItCrossesToTheLeft) {
  const Intent intent =
      ReaperBehaviour{}.steer(reaperView(FAR_RIGHT_OF_THE_POINT, std::nullopt));

  EXPECT_EQ(intent.direction.x, -FULL_STEERING);
  EXPECT_EQ(intent.direction.y, 0.0F);
}

/**
 * Given an idle Reaper 240 units to the right of the point it strikes a ship
 * from
 * When it steers
 * Then it heads for that point and starts no attack
 */
TEST(ReaperBehaviour, ItHeadsForThePointItStrikesFrom) {
  const Intent intent =
      ReaperBehaviour{}.steer(reaperView(FAR_RIGHT_OF_THE_POINT, HUNTED_SHIP));

  EXPECT_FLOAT_EQ(intent.direction.x, SHARE_TO_THE_POINT);
  EXPECT_EQ(intent.direction.y, 0.0F);
  EXPECT_FALSE(intent.startsAction.has_value());
}

/**
 * Given a rested Reaper on the point it strikes the nearest ship from
 * When it steers
 * Then it stops and raises its scythe: the signal of its attack starts
 */
TEST(ReaperBehaviour, OnItsPointARestedReaperRaisesItsScythe) {
  const Intent intent =
      ReaperBehaviour{}.steer(reaperView(STRIKING_POINT, HUNTED_SHIP));

  EXPECT_EQ(intent.startsAction, SCYTHE_SIGNAL);
  EXPECT_EQ(intent.direction.x, 0.0F);
  EXPECT_EQ(intent.direction.y, 0.0F);
}

/**
 * Given a Reaper on its point, idle for 500 ms only
 * When it steers
 * Then it starts no attack: it has not rested
 */
TEST(ReaperBehaviour, ItWaitsForItsRestBeforeItSwingsAgain) {
  BehaviourView view = reaperView(STRIKING_POINT, HUNTED_SHIP);
  view.actionAge = NOT_RESTED;

  EXPECT_FALSE(ReaperBehaviour{}.steer(view).startsAction.has_value());
}

/**
 * Given a Reaper at the start of its strike, the ship far away
 * When it steers
 * Then it stays where it is and its phase goes on
 */
TEST(ReaperBehaviour, ItStaysStillWhileItSwings) {
  BehaviourView view = reaperView(STRIKING_POINT, FAR_RIGHT_OF_THE_POINT);
  view.action = SCYTHE_STRIKE;
  view.actionAge = {};

  const Intent intent = ReaperBehaviour{}.steer(view);

  EXPECT_EQ(intent.direction.x, 0.0F);
  EXPECT_EQ(intent.direction.y, 0.0F);
  EXPECT_FALSE(intent.startsAction.has_value());
}

/**
 * Given a Reaper whose strike has lasted its 140 ms
 * When it steers
 * Then its recover starts
 */
TEST(ReaperBehaviour, ItsStrikeEndsInItsRecover) {
  BehaviourView view = reaperView(STRIKING_POINT, HUNTED_SHIP);
  view.action = SCYTHE_STRIKE;
  view.actionAge = SCYTHE_STRIKE_TIME;

  EXPECT_EQ(ReaperBehaviour{}.steer(view).startsAction, SCYTHE_RECOVER);
}

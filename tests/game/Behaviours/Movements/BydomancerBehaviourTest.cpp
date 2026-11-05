#include <gtest/gtest.h>
#include <optional>
#include "BehaviourView.hpp"
#include "BydomancerBehaviour.hpp"
#include "BydomancerBehaviourFixtures.hpp"
#include "BydomancerBehaviourTestConstants.hpp"
#include "Intent.hpp"
#include "Position.hpp"

using rtype::game::BehaviourView;
using rtype::game::BydomancerBehaviour;
using rtype::game::Intent;
using rtype::game::Position;
using rtype::game::testing::AT_ITS_COLUMN;
using rtype::game::testing::bydomancerView;
using rtype::game::testing::CLOSE_SHIP;
using rtype::game::testing::COMING_IN;
using rtype::game::testing::FAR_SHIP;
using rtype::game::testing::LEAPT;
using rtype::game::testing::NOVA;
using rtype::game::testing::ORB_SIGNAL;
using rtype::game::testing::ORB_STRIKE;
using rtype::game::testing::ROW_AFTER_THE_LEAP;

/**
 * Given a rested Bydomancer still right of its column
 * When it steers
 * Then it flies left and casts nothing yet
 */
TEST(BydomancerBehaviour, ItComesInBeforeItCasts) {
  const Intent intent =
      BydomancerBehaviour{}.steer(bydomancerView(COMING_IN, FAR_SHIP));

  EXPECT_LT(intent.direction.x, 0.0F);
  EXPECT_FALSE(intent.startsAction.has_value());
}

/**
 * Given a rested Bydomancer at its column, the ship far away
 * When it steers
 * Then it stays at its column and raises its staff for an orb
 */
TEST(BydomancerBehaviour, AtItsColumnItRaisesItsStaff) {
  const Intent intent =
      BydomancerBehaviour{}.steer(bydomancerView(AT_ITS_COLUMN, FAR_SHIP));

  EXPECT_EQ(intent.direction.x, 0.0F);
  EXPECT_EQ(intent.startsAction, ORB_SIGNAL);
}

/**
 * Given a Bydomancer in the cast of its orb
 * When it steers
 * Then it wants to fire
 */
TEST(BydomancerBehaviour, ItFiresDuringItsCast) {
  BehaviourView view = bydomancerView(AT_ITS_COLUMN, FAR_SHIP);
  view.action = ORB_STRIKE;
  view.actionAge = {};

  EXPECT_TRUE(BydomancerBehaviour{}.steer(view).shooting);
}

/**
 * Given a Bydomancer raising its staff
 * When it steers
 * Then it does not fire yet
 */
TEST(BydomancerBehaviour, ItDoesNotFireWhileItRaisesItsStaff) {
  BehaviourView view = bydomancerView(AT_ITS_COLUMN, FAR_SHIP);
  view.action = ORB_SIGNAL;
  view.actionAge = {};

  EXPECT_FALSE(BydomancerBehaviour{}.steer(view).shooting);
}

/**
 * Given an idle Bydomancer and a ship 100 units away
 * When it steers
 * Then it casts its nova
 */
TEST(BydomancerBehaviour, AShipCloseByMakesItCastItsNova) {
  const Intent intent =
      BydomancerBehaviour{}.steer(bydomancerView(AT_ITS_COLUMN, CLOSE_SHIP));

  EXPECT_EQ(intent.startsAction, NOVA);
}

/**
 * Given a wounded Bydomancer still on the row it appeared on, in the upper
 * half of the area of play
 * When it steers
 * Then it leaps 400 units lower, at once
 */
TEST(BydomancerBehaviour, WoundedItLeapsToAnotherRow) {
  BehaviourView view = bydomancerView(AT_ITS_COLUMN, FAR_SHIP);
  view.wounded = true;

  const std::optional<Position> leap =
      BydomancerBehaviour{}.steer(view).jumpsTo;

  ASSERT_TRUE(leap.has_value());
  EXPECT_EQ(leap.value_or(Position{}).y, ROW_AFTER_THE_LEAP);
  EXPECT_EQ(leap.value_or(Position{}).x, AT_ITS_COLUMN.x);
}

/**
 * Given a wounded Bydomancer that already leapt to its new row
 * When it steers
 * Then it does not leap again
 */
TEST(BydomancerBehaviour, ItLeapsOnlyOnce) {
  BehaviourView view = bydomancerView(LEAPT, FAR_SHIP);
  view.wounded = true;

  EXPECT_FALSE(BydomancerBehaviour{}.steer(view).jumpsTo.has_value());
}

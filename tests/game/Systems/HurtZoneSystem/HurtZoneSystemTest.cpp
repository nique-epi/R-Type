#include <gtest/gtest.h>
#include "HurtZone.hpp"
#include "HurtZoneSystemFixture.hpp"
#include "HurtZoneSystemTestConstants.hpp"

using rtype::game::HurtBox;
using rtype::game::HurtZone;
using rtype::game::testing::ALWAYS_ACTIVE_BOX;
using rtype::game::testing::HurtZoneSystemFixture;
using rtype::game::testing::OTHER_STRIKE;
using rtype::game::testing::SCYTHE_SIGNAL;
using rtype::game::testing::SCYTHE_STRIKE;
using rtype::game::testing::SHIP_FAR_AWAY_X;
using rtype::game::testing::SHIP_IN_THE_BOX_X;
using rtype::game::testing::SHIP_ON_THE_ENEMY_X;

/**
 * Given an enemy whose always-active box lies 60 units to its left, and a ship
 * inside that box
 * When the system runs
 * Then one ShipContact names that ship and that enemy
 */
TEST_F(HurtZoneSystemFixture, AShipInsideABoxIsInContact) {
  addEnemy(HurtZone{.boxes = {ALWAYS_ACTIVE_BOX}});
  addShip(SHIP_IN_THE_BOX_X);

  EXPECT_EQ(run().size(), 1U);
}

/**
 * Given the same enemy and a ship on the enemy itself, outside the offset box
 * When the system runs
 * Then there is no contact: the box is where it is offset to, not on the body
 */
TEST_F(HurtZoneSystemFixture, TheBoxIsOffsetFromTheBody) {
  addEnemy(HurtZone{.boxes = {ALWAYS_ACTIVE_BOX}});
  addShip(SHIP_ON_THE_ENEMY_X);

  EXPECT_TRUE(run().empty());
}

/**
 * Given an enemy with a box and a ship far from it
 * When the system runs
 * Then there is no contact
 */
TEST_F(HurtZoneSystemFixture, AShipFarFromEveryBoxIsNotInContact) {
  addEnemy(HurtZone{.boxes = {ALWAYS_ACTIVE_BOX}});
  addShip(SHIP_FAR_AWAY_X);

  EXPECT_TRUE(run().empty());
}

/**
 * Given a box active in the Strike of the scythe attack, and a ship in it
 * When the enemy is idle, then in the Signal of that attack, then in the
 * Strike of another attack
 * Then the ship is not in contact
 */
TEST_F(HurtZoneSystemFixture, ABoxHurtsOnlyInItsOwnAttackAndPhase) {
  HurtBox box = ALWAYS_ACTIVE_BOX;
  box.activeIn = SCYTHE_STRIKE;
  addEnemy(HurtZone{.boxes = {box}});
  addShip(SHIP_IN_THE_BOX_X);

  EXPECT_TRUE(run().empty());
  enterAction(SCYTHE_SIGNAL);
  EXPECT_TRUE(run().empty());
  enterAction(OTHER_STRIKE);
  EXPECT_TRUE(run().empty());
}

/**
 * Given a box active in the Strike of the scythe attack, and a ship in it
 * When the enemy enters that Strike
 * Then the ship is in contact
 */
TEST_F(HurtZoneSystemFixture, ABoxHurtsInItsOwnAttackAndPhase) {
  HurtBox box = ALWAYS_ACTIVE_BOX;
  box.activeIn = SCYTHE_STRIKE;
  addEnemy(HurtZone{.boxes = {box}});
  addShip(SHIP_IN_THE_BOX_X);
  enterAction(SCYTHE_STRIKE);

  EXPECT_EQ(run().size(), 1U);
}

/**
 * Given two boxes that both cover a ship
 * When the system runs
 * Then the ship is in contact once, not twice
 */
TEST_F(HurtZoneSystemFixture, SeveralBoxesOnOneShipMakeOneContact) {
  addEnemy(HurtZone{.boxes = {ALWAYS_ACTIVE_BOX, ALWAYS_ACTIVE_BOX}});
  addShip(SHIP_IN_THE_BOX_X);

  EXPECT_EQ(run().size(), 1U);
}

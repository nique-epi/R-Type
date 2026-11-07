#include <gtest/gtest.h>
#include "Entity.hpp"
#include "LagCompensatedHitSystemShipsFixture.hpp"
#include "LagCompensatedHitSystemShipsTestConstants.hpp"

using rtype::engine::Entity;
using rtype::game::testing::LagCompensatedHitSystemShipsFixture;
using rtype::game::testing::SHIP_FIVE_TICKS_BEHIND;
using rtype::game::testing::TICKS_AT_THE_FIRST_PLACE;
using rtype::game::testing::TICKS_AT_THE_SECOND_PLACE;
using rtype::game::testing::TWO_HUNDRED_UNITS_HIGHER;
using rtype::game::testing::WHERE_THE_SHIP_IS;

/**
 * Given a missile of an enemy that stood on the ship for one tick, then went
 * 200 units higher for five ticks
 * When the ship is tested, its player five ticks behind
 * Then the missile hits the ship: its player saw the missile on it
 */
TEST_F(LagCompensatedHitSystemShipsFixture,
       AShipIsHitByAMissileWhereItsPlayerSawIt) {
  const Entity missile = enemyMissileAt(WHERE_THE_SHIP_IS);
  runTicks(TICKS_AT_THE_FIRST_PLACE);
  move(missile, TWO_HUNDRED_UNITS_HIGHER);
  runTicks(TICKS_AT_THE_SECOND_PLACE - 1);

  const Entity ship = shipAt(WHERE_THE_SHIP_IS, SHIP_FIVE_TICKS_BEHIND);
  runTicks(1);

  ASSERT_EQ(hits().size(), 1U);
  EXPECT_EQ(hits().front().missile, missile);
  EXPECT_EQ(hits().front().target, ship);
}

/**
 * Given a missile of an enemy 200 units higher for one tick, then on the ship
 * for five ticks
 * When the ship is tested, its player five ticks behind
 * Then nothing hits the ship: its player saw the missile miss it, and the
 * server takes the dodge it saw
 */
TEST_F(LagCompensatedHitSystemShipsFixture,
       AShipDodgesAMissileItsPlayerSawMiss) {
  const Entity missile = enemyMissileAt(TWO_HUNDRED_UNITS_HIGHER);
  runTicks(TICKS_AT_THE_FIRST_PLACE);
  move(missile, WHERE_THE_SHIP_IS);
  runTicks(TICKS_AT_THE_SECOND_PLACE - 1);

  static_cast<void>(shipAt(WHERE_THE_SHIP_IS, SHIP_FIVE_TICKS_BEHIND));
  runTicks(1);

  EXPECT_TRUE(hits().empty());
}

/**
 * Given a missile of an enemy that stood on the ship for six ticks, destroyed
 * just before the ship is tested
 * When the ship is tested, its player five ticks behind
 * Then nothing hits the ship: a missile already gone never hits
 */
TEST_F(LagCompensatedHitSystemShipsFixture, AMissileAlreadyGoneNeverHitsAShip) {
  const Entity missile = enemyMissileAt(WHERE_THE_SHIP_IS);
  runTicks(TICKS_AT_THE_FIRST_PLACE + TICKS_AT_THE_SECOND_PLACE - 1);
  static_cast<void>(shipAt(WHERE_THE_SHIP_IS, SHIP_FIVE_TICKS_BEHIND));
  components().destroy(missile);

  runTicks(1);

  EXPECT_TRUE(hits().empty());
}

/**
 * Given an enemy whose HurtZone stood on the ship for one tick, then went 200
 * units higher for five ticks
 * When the ship is tested, its player five ticks behind
 * Then the ship touches that enemy: its player saw the zone on it
 */
TEST_F(LagCompensatedHitSystemShipsFixture,
       AShipIsHurtByAZoneWhereItsPlayerSawIt) {
  const Entity enemy = hurtZoneAt(WHERE_THE_SHIP_IS);
  runTicks(TICKS_AT_THE_FIRST_PLACE);
  move(enemy, TWO_HUNDRED_UNITS_HIGHER);
  runTicks(TICKS_AT_THE_SECOND_PLACE - 1);

  const Entity ship = shipAt(WHERE_THE_SHIP_IS, SHIP_FIVE_TICKS_BEHIND);
  runTicks(1);

  ASSERT_EQ(contacts().size(), 1U);
  EXPECT_EQ(contacts().front().player, ship);
  EXPECT_EQ(contacts().front().enemy, enemy);
}

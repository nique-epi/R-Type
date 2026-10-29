#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntityIdConstants.hpp"
#include "EntitySpawned.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "MissileStartPosition.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "TimeConstants.hpp"
#include "WeaponSystemFixture.hpp"
#include "WeaponSystemTestConstants.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {
namespace {

/**
 * Given a player with a ship who holds shoot
 * When one tick runs
 * Then one missile is in the world
 */
TEST_F(WeaponSystemFixture, ShipFiresAtOnceWhenShootIsHeld) {
  runTicksShooting(connect(FIRST_PLAYER), 1);

  EXPECT_EQ(missilesInTheWorld(), 1U);
}

/**
 * Given a player with a ship who holds shoot
 * When one tick runs
 * Then the player is sent the EntitySpawned of a missile of a player, in front
 * of its ship
 */
TEST_F(WeaponSystemFixture, AnnouncesTheMissileInFrontOfTheShip) {
  ConnectedPlayer& player = connect(FIRST_PLAYER);

  runTicksShooting(player, 1);

  const std::vector<network::EntitySpawned> spawns = spawnsSentTo(FIRST_PLAYER);
  ASSERT_EQ(spawns.size(), 1U);
  const game::Position start = game::missileStartPosition(shipOf(player));
  EXPECT_EQ(spawns.front().entityType, network::WireEntityType::PlayerMissile);
  EXPECT_NEAR(spawns.front().x, start.x, WIRE_COORDINATE_TOLERANCE);
  EXPECT_NEAR(spawns.front().y, start.y, WIRE_COORDINATE_TOLERANCE);
}

/**
 * Given two players with a ship, the first holding shoot
 * When one tick runs
 * Then the second player is sent the EntitySpawned of that missile too
 */
TEST_F(WeaponSystemFixture, TellsEveryPlayerAboutAMissile) {
  ConnectedPlayer& shooter = connect(FIRST_PLAYER);
  static_cast<void>(connect(SECOND_PLAYER));

  runTicksShooting(shooter, 1);

  EXPECT_EQ(spawnsSentTo(SECOND_PLAYER), spawnsSentTo(FIRST_PLAYER));
  EXPECT_EQ(spawnsSentTo(SECOND_PLAYER).size(), 1U);
}

/**
 * Given two players with a ship, the second holding shoot
 * When one tick runs
 * Then the missile belongs to the second player
 */
TEST_F(WeaponSystemFixture, GivesTheMissileToThePlayerWhoFired) {
  static_cast<void>(connect(FIRST_PLAYER));
  ConnectedPlayer& shooter = connect(SECOND_PLAYER);

  runTicksShooting(shooter, 1);

  EXPECT_EQ(ownersOfTheMissiles(),
            std::vector<std::uint8_t>{shooter.playerNumber});
}

/**
 * Given two players with a ship, neither holding shoot
 * When ticks run
 * Then no missile appears and nothing is sent
 */
TEST_F(WeaponSystemFixture, FiresNothingWithoutAShooter) {
  static_cast<void>(connect(FIRST_PLAYER));
  static_cast<void>(connect(SECOND_PLAYER));

  runTicksWithoutShooting(TICKS_BETWEEN_TWO_SHOTS);

  EXPECT_EQ(missilesInTheWorld(), 0U);
  EXPECT_TRUE(spawnsSentTo(FIRST_PLAYER).empty());
}

/**
 * Given a ship that fired, then belongs to no connected player, its Intent
 * still wanting to shoot
 * When its wait of 20 ticks is over and one more tick runs
 * Then no missile appears: a missile of a player always has an owner
 */
TEST_F(WeaponSystemFixture, ShipOfNoConnectedPlayerFiresNothing) {
  ConnectedPlayer& player = connect(FIRST_PLAYER);
  runTicksShooting(player, 1);
  const std::size_t firedByTheOwner = missilesInTheWorld();
  player.shipEntityId = identifiers::NO_ENTITY_ID;

  runTicks(TICKS_BETWEEN_TWO_SHOTS + 1);

  EXPECT_EQ(firedByTheOwner, 1U);
  EXPECT_EQ(missilesInTheWorld(), firedByTheOwner);
}

/**
 * Given a player with a ship who holds shoot
 * When a second of ticks runs
 * Then three missiles appeared, the rate chosen for the game
 */
TEST_F(WeaponSystemFixture, ShipFiresThreeMissilesPerSecondWhileShootIsHeld) {
  runTicksShooting(connect(FIRST_PLAYER), engine::SIMULATION_TICKS_PER_SECOND);

  EXPECT_EQ(missilesInTheWorld(), MISSILES_PER_SECOND_OF_FIRE);
}

/**
 * Given a player with a ship who holds shoot
 * When 20 ticks run, then one more
 * Then one missile appeared in the 20 ticks, and the second at the 21st
 */
TEST_F(WeaponSystemFixture, ShipWaitsTwentyTicksBetweenTwoMissiles) {
  ConnectedPlayer& shooter = connect(FIRST_PLAYER);

  runTicksShooting(shooter, TICKS_BETWEEN_TWO_SHOTS);
  EXPECT_EQ(missilesInTheWorld(), 1U);
  runTicksShooting(shooter, 1);

  EXPECT_EQ(missilesInTheWorld(), 2U);
}

/**
 * Given a player who fired, then releases and presses shoot again at every
 * tick, as a modified client could
 * When the rest of the 20 ticks runs
 * Then no second missile appeared: pressing again does not shorten the wait
 */
TEST_F(WeaponSystemFixture, PressingShootAgainFiresNoSooner) {
  ConnectedPlayer& shooter = connect(FIRST_PLAYER);
  runTicksShooting(shooter, 1);

  bool holdsShoot = false;
  for (std::size_t tick = 1; tick < TICKS_BETWEEN_TWO_SHOTS; ++tick) {
    if (holdsShoot) {
      runTicksShooting(shooter, 1);
    } else {
      runTicksWithoutShooting(1);
    }
    holdsShoot = !holdsShoot;
  }

  EXPECT_EQ(missilesInTheWorld(), 1U);
}

/**
 * Given a player who fired, then released shoot for 20 ticks
 * When it holds shoot again for one tick
 * Then a second missile appears at once
 */
TEST_F(WeaponSystemFixture, ShipFiresAtOnceWhenShootIsPressedAfterTheWait) {
  ConnectedPlayer& shooter = connect(FIRST_PLAYER);
  runTicksShooting(shooter, 1);
  runTicksWithoutShooting(TICKS_BETWEEN_TWO_SHOTS);

  runTicksShooting(shooter, 1);

  EXPECT_EQ(missilesInTheWorld(), 2U);
}

/**
 * Given a first player who just fired, and a second player who starts
 * holding shoot at the next tick
 * When that tick runs
 * Then the second player fires at once: each ship waits on its own
 */
TEST_F(WeaponSystemFixture, EachShipWaitsOnItsOwn) {
  ConnectedPlayer& first = connect(FIRST_PLAYER);
  ConnectedPlayer& second = connect(SECOND_PLAYER);
  runTicksShooting(first, 1);

  runTicksShooting(second, 1);

  EXPECT_EQ(missilesInTheWorld(), 2U);
}

/**
 * Given a Bydo that just appeared
 * When the ticks of one and a half seconds but one run
 * Then it fired nothing yet
 */
TEST_F(WeaponSystemFixture, BydoFiresNothingBeforeOneAndAHalfSeconds) {
  bydoAt(SOME_BYDO);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS - 1);

  EXPECT_TRUE(enemyMissilesInTheWorld().empty());
}

/**
 * Given a Bydo that just appeared
 * When the ticks of one and a half seconds run
 * Then it fired one missile
 */
TEST_F(WeaponSystemFixture, BydoFiresOneAndAHalfSecondsAfterItAppears) {
  bydoAt(SOME_BYDO);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS);

  EXPECT_EQ(enemyMissilesInTheWorld().size(), 1U);
}

/**
 * Given a Bydo that just fired its first missile
 * When the ticks of one and a half seconds but one run
 * Then it fired no second missile yet
 */
TEST_F(WeaponSystemFixture, BydoWaitsOneAndAHalfSecondsBetweenTwoMissiles) {
  bydoAt(SOME_BYDO);
  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS - 1);

  EXPECT_EQ(enemyMissilesInTheWorld().size(), 1U);
}

/**
 * Given a Bydo that just fired its first missile
 * When the ticks of one and a half seconds run again
 * Then it fired a second missile
 */
TEST_F(WeaponSystemFixture, BydoFiresAgainEveryOneAndAHalfSeconds) {
  bydoAt(SOME_BYDO);
  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS);

  EXPECT_EQ(enemyMissilesInTheWorld().size(), 2U);
}

/**
 * Given a Bydo, and a second one that appeared half a second later
 * When one and a half seconds passed since the first appeared
 * Then only the first fired: each Bydo counts from its own appearance
 */
TEST_F(WeaponSystemFixture, EachBydoCountsFromItsOwnAppearance) {
  bydoAt(SOME_BYDO);
  runTicks(HALF_A_SECOND_IN_TICKS);
  bydoAt(OTHER_BYDO);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS - HALF_A_SECOND_IN_TICKS);

  const std::vector<network::EntitySpawned> missiles =
      enemyMissilesInTheWorld();
  ASSERT_EQ(missiles.size(), 1U);
  EXPECT_FLOAT_EQ(missiles.front().y, SOME_BYDO.y);
}

/**
 * Given a Bydo
 * When it fires
 * Then its missile starts on its row, its right side against the left side of
 * the Bydo
 */
TEST_F(WeaponSystemFixture, BydoFiresFromTheFrontOfTheBydo) {
  bydoAt(SOME_BYDO);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS);

  const std::vector<network::EntitySpawned> missiles =
      enemyMissilesInTheWorld();
  ASSERT_EQ(missiles.size(), 1U);
  EXPECT_FLOAT_EQ(missiles.front().x, SOME_BYDO_MISSILE_X);
  EXPECT_FLOAT_EQ(missiles.front().y, SOME_BYDO.y);
}

/**
 * Given two connected players and a Bydo
 * When it fires
 * Then each player is sent, in a datagram of its own, the EntitySpawned of the
 * missile of an enemy, under its network id and at its position
 */
TEST_F(WeaponSystemFixture, TellsEveryPlayerAboutTheMissileOfABydo) {
  connectWithoutShip(FIRST_PLAYER);
  connectWithoutShip(SECOND_PLAYER);
  bydoAt(SOME_BYDO);

  runTicks(ONE_AND_A_HALF_SECONDS_IN_TICKS);

  const std::vector<network::EntitySpawned> missiles =
      enemyMissilesInTheWorld();
  ASSERT_EQ(missiles.size(), 1U);
  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    const std::vector<network::OutgoingDatagram> received =
        datagramsSentTo(player);
    ASSERT_EQ(received.size(), 1U);
    EXPECT_EQ(onlyMessageIn<network::EntitySpawned>(received.front()),
              missiles.front());
  }
}

/**
 * Given a ship that holds shoot and a Bydo that just appeared
 * When one tick runs
 * Then the ship fired its missile and the Bydo fired nothing: the same system
 * gives each its own first shot
 */
TEST_F(WeaponSystemFixture, ShipAndBydoHaveTheirOwnFirstShot) {
  bydoAt(SOME_BYDO);

  runTicksShooting(connect(FIRST_PLAYER), 1);

  EXPECT_EQ(missilesInTheWorld(), 1U);
  EXPECT_TRUE(enemyMissilesInTheWorld().empty());
}

/**
 * Given a ship that holds shoot and a Bydo, both under the same system
 * When one and a half seconds run
 * Then the ship fired a missile every 20 ticks, five times, and the Bydo
 * once, at the 90th tick
 */
TEST_F(WeaponSystemFixture, ShipAndBydoFireAtTheirOwnCadence) {
  bydoAt(SOME_BYDO);

  runTicksShooting(connect(FIRST_PLAYER), ONE_AND_A_HALF_SECONDS_IN_TICKS);

  EXPECT_EQ(missilesInTheWorld(), SHIPS_MISSILES_IN_ONE_AND_A_HALF_SECONDS);
  EXPECT_EQ(enemyMissilesInTheWorld().size(), 1U);
}

}  // namespace
}  // namespace rtype::server::testing

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntityIdConstants.hpp"
#include "EntitySpawned.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "MissileLauncherFixture.hpp"
#include "MissileLauncherTestConstants.hpp"
#include "MissileStartPosition.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {
namespace {

/**
 * Given a player with a ship who holds shoot
 * When one tick runs
 * Then one missile is in the world
 */
TEST_F(MissileLauncherFixture, FiresAtOnceWhenShootIsHeld) {
  runTicksShooting(connect(FIRST_PLAYER), 1);

  EXPECT_EQ(missilesInTheWorld(), 1U);
}

/**
 * Given a player with a ship who holds shoot
 * When one tick runs
 * Then the player is sent the EntitySpawned of a missile of a player, in front
 * of its ship
 */
TEST_F(MissileLauncherFixture, AnnouncesTheMissileInFrontOfTheShip) {
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
TEST_F(MissileLauncherFixture, TellsEveryPlayerAboutAMissile) {
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
TEST_F(MissileLauncherFixture, GivesTheMissileToThePlayerWhoFired) {
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
TEST_F(MissileLauncherFixture, FiresNothingWithoutAShooter) {
  static_cast<void>(connect(FIRST_PLAYER));
  static_cast<void>(connect(SECOND_PLAYER));

  runTicksWithoutShooting(TICKS_BETWEEN_TWO_SHOTS);

  EXPECT_EQ(missilesInTheWorld(), 0U);
  EXPECT_TRUE(spawnsSentTo(FIRST_PLAYER).empty());
}

/**
 * Given a player who holds shoot but has no ship
 * When ticks run
 * Then no missile appears
 */
TEST_F(MissileLauncherFixture, FiresNothingWithoutAShip) {
  ConnectedPlayer& player = connect(FIRST_PLAYER);
  player.shipEntityId = identifiers::NO_ENTITY_ID;

  runTicksShooting(player, TICKS_BETWEEN_TWO_SHOTS);

  EXPECT_EQ(missilesInTheWorld(), 0U);
}

/**
 * Given a player with a ship who holds shoot
 * When a second of ticks runs
 * Then three missiles appeared, the rate chosen for the game
 */
TEST_F(MissileLauncherFixture, FiresThreeMissilesPerSecondWhileShootIsHeld) {
  runTicksShooting(connect(FIRST_PLAYER), engine::SIMULATION_TICKS_PER_SECOND);

  EXPECT_EQ(missilesInTheWorld(), MISSILES_PER_SECOND_OF_FIRE);
}

/**
 * Given a player with a ship who holds shoot
 * When 20 ticks run, then one more
 * Then one missile appeared in the 20 ticks, and the second at the 21st
 */
TEST_F(MissileLauncherFixture, WaitsTwentyTicksBetweenTwoMissiles) {
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
TEST_F(MissileLauncherFixture, PressingShootAgainFiresNoSooner) {
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
TEST_F(MissileLauncherFixture, FiresAtOnceWhenShootIsPressedAfterTheWait) {
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
TEST_F(MissileLauncherFixture, EachShipWaitsOnItsOwn) {
  ConnectedPlayer& first = connect(FIRST_PLAYER);
  ConnectedPlayer& second = connect(SECOND_PLAYER);
  runTicksShooting(first, 1);

  runTicksShooting(second, 1);

  EXPECT_EQ(missilesInTheWorld(), 2U);
}

}  // namespace
}  // namespace rtype::server::testing

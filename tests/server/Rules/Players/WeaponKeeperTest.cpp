#include <gtest/gtest.h>
#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "EntityTypesTestConstants.hpp"
#include "Gun.hpp"
#include "WeaponKeeperFixture.hpp"
#include "WeaponKeeperTestConstants.hpp"
#include "WeaponPickupTestConstants.hpp"

using rtype::game::testing::CRYSTAL_RED_SHEET;

namespace rtype::server::testing {

/**
 * Given a player whose ship touched a red crystal, delivered by the bus
 * When the next tick runs
 * Then its ship holds the first level of red and the crystal is removed once
 */
TEST_F(WeaponKeeperFixture, AShipTakesTheWeaponOfTheCrystalItTouches) {
  const ConnectedPlayer& player = join(FIRST_PILOT);
  const std::uint32_t crystal = spawnCrystal(CRYSTAL_RED_SHEET);
  touch(player, crystal);

  tick();

  EXPECT_EQ(gunOf(player).weapon, RED_WEAPON_NUMBER);
  EXPECT_EQ(gunOf(player).level, FIRST_LEVEL);
  EXPECT_FALSE(worldHolds(crystal));
  EXPECT_EQ(removalsOf(crystal), 1U);
}

/**
 * Given a player whose ship touched a red crystal, delivered by the bus
 * When no tick ran yet
 * Then its ship still holds its gun and the crystal is still there
 */
TEST_F(WeaponKeeperFixture, ACrystalIsGivenAtTheNextTick) {
  const ConnectedPlayer& player = join(FIRST_PILOT);
  const std::uint32_t crystal = spawnCrystal(CRYSTAL_RED_SHEET);

  touch(player, crystal);

  EXPECT_EQ(gunOf(player).weapon, GUN_OF_THE_SHEET);
  EXPECT_TRUE(worldHolds(crystal));
}

/**
 * Given two players whose ships touched the same red crystal in one tick, the
 * second first
 * When the next tick runs
 * Then the first player, the lowest number, takes red, and the second keeps
 * its gun
 */
TEST_F(WeaponKeeperFixture, TheLowestPlayerNumberWinsACrystalTouchedByTwo) {
  const ConnectedPlayer& first = join(FIRST_PILOT);
  const ConnectedPlayer& second = join(SECOND_PILOT);
  const std::uint32_t crystal = spawnCrystal(CRYSTAL_RED_SHEET);
  touch(second, crystal);
  touch(first, crystal);

  tick();

  EXPECT_EQ(gunOf(first).weapon, RED_WEAPON_NUMBER);
  EXPECT_EQ(gunOf(second).weapon, GUN_OF_THE_SHEET);
  EXPECT_EQ(removalsOf(crystal), 1U);
}

/**
 * Given a player who took red, then touched a second red crystal
 * When the next tick runs
 * Then its ship holds the second level of red
 */
TEST_F(WeaponKeeperFixture, ASecondCrystalOfTheSameColorRaisesTheLevel) {
  const ConnectedPlayer& player = join(FIRST_PILOT);
  touch(player, spawnCrystal(CRYSTAL_RED_SHEET));
  tick();
  touch(player, spawnCrystal(CRYSTAL_RED_SHEET));

  tick();

  EXPECT_EQ(gunOf(player).level, SECOND_LEVEL);
}

}  // namespace rtype::server::testing

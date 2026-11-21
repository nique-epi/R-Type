#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "ShippedEntitySheets.hpp"
#include "WaveCatalog.hpp"
#include "WaveCatalogEnemyStrengthTestConstants.hpp"
#include "WaveCatalogFixtures.hpp"
#include "WaveCatalogTestConstants.hpp"

using rtype::game::testing::shippedEntitySheets;
using rtype::server::WaveCatalog;
using rtype::server::testing::calmFile;
using rtype::server::testing::calmFileWith;
using rtype::server::testing::EASY_ENEMY_STRENGTH;
using rtype::server::testing::ENEMY_GUN_INTERVAL_PERCENT_IN_TESTS;
using rtype::server::testing::ENEMY_HEALTH_PERCENT_IN_TESTS;
using rtype::server::testing::errorReading;
using rtype::server::testing::HARD_ENEMY_STRENGTH;
using rtype::server::testing::NORMAL_ENEMY_STRENGTH;
using rtype::server::testing::ONE_HUNDRED_AND_FIFTY_AS_A_VALUE;
using rtype::server::testing::ONE_HUNDRED_AND_FIFTY_PERCENT;
using rtype::server::testing::shippedCatalog;
using rtype::server::testing::TEST_FILE_NAME;
using rtype::server::testing::ZERO_AS_A_VALUE;
using ::testing::HasSubstr;

/**
 * Given a waves file whose difficulty calm gives enemy_health_percent = 150
 * When it is read
 * Then the enemies of calm are made at 150 % of their health
 */
TEST(WaveCatalog, ReadsTheHealthPercentOfTheEnemies) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(calmFileWith(ENEMY_HEALTH_PERCENT_IN_TESTS,
                                         ONE_HUNDRED_AND_FIFTY_AS_A_VALUE),
                            TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(catalog.planOf("calm").enemyStrength.healthPercent,
            ONE_HUNDRED_AND_FIFTY_PERCENT);
}

/**
 * Given a waves file whose difficulty calm gives
 * enemy_gun_interval_percent = 150
 * When it is read
 * Then the guns of the enemies of calm wait 150 % of their ticks
 */
TEST(WaveCatalog, ReadsTheGunIntervalPercentOfTheEnemies) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(calmFileWith(ENEMY_GUN_INTERVAL_PERCENT_IN_TESTS,
                                         ONE_HUNDRED_AND_FIFTY_AS_A_VALUE),
                            TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(catalog.planOf("calm").enemyStrength.gunIntervalPercent,
            ONE_HUNDRED_AND_FIFTY_PERCENT);
}

/**
 * Given a waves file whose difficulty calm gives neither percent
 * When it is read
 * Then the enemies of calm are made as their sheets give them
 */
TEST(WaveCatalog, ADifficultyWithoutThePercentsKeepsTheSheets) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(calmFile(), TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(catalog.planOf("calm").enemyStrength, NORMAL_ENEMY_STRENGTH);
}

/**
 * Given a waves file whose difficulty calm gives enemy_health_percent = 0
 * When it is read
 * Then it is refused, naming the key: an enemy needs some health
 */
TEST(WaveCatalog, RefusesAnEnemyHealthPercentOfZero) {
  EXPECT_THAT(errorReading(
                  calmFileWith(ENEMY_HEALTH_PERCENT_IN_TESTS, ZERO_AS_A_VALUE)),
              HasSubstr(ENEMY_HEALTH_PERCENT_IN_TESTS));
}

/**
 * Given the waves file the game ships
 * When it is read
 * Then easy makes its enemies weaker and slower to fire
 */
TEST(WaveCatalog, EasyMakesItsEnemiesWeakerAndSlowerToFire) {
  EXPECT_EQ(shippedCatalog().planOf("easy").enemyStrength, EASY_ENEMY_STRENGTH);
}

/**
 * Given the waves file the game ships
 * When it is read
 * Then normal makes its enemies as their sheets give them
 */
TEST(WaveCatalog, NormalMakesItsEnemiesAsTheirSheets) {
  EXPECT_EQ(shippedCatalog().planOf("normal").enemyStrength,
            NORMAL_ENEMY_STRENGTH);
}

/**
 * Given the waves file the game ships
 * When it is read
 * Then hard makes its enemies more solid and quicker to fire
 */
TEST(WaveCatalog, HardMakesItsEnemiesStrongerAndQuickerToFire) {
  EXPECT_EQ(shippedCatalog().planOf("hard").enemyStrength, HARD_ENEMY_STRENGTH);
}

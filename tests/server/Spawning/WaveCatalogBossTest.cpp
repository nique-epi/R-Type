#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "BossStageTestConstants.hpp"
#include "EntityTypesTestConstants.hpp"
#include "WaveCatalog.hpp"
#include "WaveCatalogBossFixtures.hpp"
#include "WaveCatalogBossTestConstants.hpp"
#include "WaveCatalogFixtures.hpp"
#include "WaveCatalogTestConstants.hpp"

using rtype::game::testing::BYDO_TYRANT_SHEET;
using rtype::server::WaveCatalog;
using rtype::server::testing::BOSS_KEY_IN_TESTS;
using rtype::server::testing::calmFile;
using rtype::server::testing::calmFileWith;
using rtype::server::testing::DRIFTER_AS_A_BOSS;
using rtype::server::testing::errorReading;
using rtype::server::testing::HARD_BOSS_HEALTH_PERCENT;
using rtype::server::testing::NORMAL_BOSS_HEALTH_PERCENT;
using rtype::server::testing::sheetsWithAWarlord;
using rtype::server::testing::shippedCatalog;
using rtype::server::testing::TEST_FILE_NAME;
using rtype::server::testing::UNKNOWN_DRIFTER_BOSS_TEXT;
using rtype::server::testing::WARLORD_AS_A_VALUE;
using rtype::server::testing::WARLORD_NAME_IN_TESTS;
using ::testing::HasSubstr;

/**
 * Given a waves file whose difficulty calm names the warlord as its boss
 * When it is read with sheets that hold the warlord
 * Then the waves of calm end with the warlord
 */
TEST(WaveCatalog, ReadsTheBossOfADifficulty) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(calmFileWith(BOSS_KEY_IN_TESTS, WARLORD_AS_A_VALUE),
                            TEST_FILE_NAME, sheetsWithAWarlord());

  EXPECT_EQ(catalog.planOf("calm").boss,
            std::optional<std::string>{WARLORD_NAME_IN_TESTS});
}

/**
 * Given a waves file whose difficulty calm names no boss
 * When it is read
 * Then the waves of calm end with their last wave
 */
TEST(WaveCatalog, ReadsADifficultyWithoutBoss) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(calmFile(), TEST_FILE_NAME, sheetsWithAWarlord());

  EXPECT_FALSE(catalog.planOf("calm").boss.has_value());
}

/**
 * Given a waves file whose difficulty calm names the Drifter as its boss
 * When it is read
 * Then it is refused: the Drifter has no table [gameplay.boss]
 */
TEST(WaveCatalog, RefusesABossThatIsNoBoss) {
  EXPECT_THAT(errorReading(calmFileWith(BOSS_KEY_IN_TESTS, DRIFTER_AS_A_BOSS)),
              HasSubstr(UNKNOWN_DRIFTER_BOSS_TEXT));
}

/**
 * Given the waves file the game ships
 * When it is read
 * Then the difficulty normal, which every game plays, ends with the Bydo
 * Tyrant
 */
TEST(WaveCatalog, NormalEndsWithTheBydoTyrant) {
  EXPECT_EQ(shippedCatalog().planOf("normal").boss,
            std::optional<std::string>{BYDO_TYRANT_SHEET});
}

/**
 * Given the waves file the game ships, whose normal gives no
 * boss_health_percent
 * When it is read
 * Then the boss of normal keeps the health of its sheet: 100 %
 */
TEST(WaveCatalog, NormalKeepsTheHealthOfItsBossSheet) {
  EXPECT_EQ(shippedCatalog().planOf("normal").bossHealthPercent,
            NORMAL_BOSS_HEALTH_PERCENT);
}

/**
 * Given the waves file the game ships, whose hard gives
 * boss_health_percent = 130
 * When it is read
 * Then the boss of hard has 130 % of the health of its sheet
 */
TEST(WaveCatalog, HardMakesItsBossMoreSolid) {
  EXPECT_EQ(shippedCatalog().planOf("hard").bossHealthPercent,
            HARD_BOSS_HEALTH_PERCENT);
}

#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "EntityTypesTestConstants.hpp"
#include "GameAssetPathsFixtures.hpp"
#include "GameAssetPathsTestConstants.hpp"
#include "GameRulesTestConstants.hpp"
#include "WaveCatalog.hpp"
#include "WavePlan.hpp"

using rtype::game::testing::BYDO_TYRANT_SHEET;
using rtype::server::WaveCatalog;
using rtype::server::WavePlan;
using rtype::server::testing::wavesOfTheAssets;
using rtype::server::testing::wavesWithTheExampleResource;
using rtype::testing::CHOSEN_DIFFICULTY;
using rtype::testing::EASY_DIFFICULTY;
using rtype::testing::NORMAL_DIFFICULTY;

/**
 * Given the assets and the example resource, whose waves give the enemies of
 * normal
 * When the waves are read with the resource started
 * Then normal sends the enemies of the resource, crimson Drifters included
 */
TEST(GameAssetPaths, TheExampleResourceGivesTheEnemiesOfNormal) {
  const WaveCatalog catalog = wavesWithTheExampleResource();

  EXPECT_EQ(catalog.planOf(NORMAL_DIFFICULTY).enemies,
            std::vector<std::string>(EXAMPLE_RESOURCE_NORMAL_ENEMIES.begin(),
                                     EXAMPLE_RESOURCE_NORMAL_ENEMIES.end()));
}

/**
 * Given the assets, whose normal and hard end with the Bydo Tyrant, and the
 * example resource, whose waves name no boss
 * When the waves are read with the resource started
 * Then normal and hard still end with the Bydo Tyrant
 */
TEST(GameAssetPaths, TheTyrantStillEndsNormalAndHardWithTheExampleResource) {
  const WaveCatalog catalog = wavesWithTheExampleResource();
  const std::optional<std::string> tyrant{std::string{BYDO_TYRANT_SHEET}};

  EXPECT_EQ(catalog.planOf(NORMAL_DIFFICULTY).boss, tyrant);
  EXPECT_EQ(catalog.planOf(CHOSEN_DIFFICULTY).boss, tyrant);
}

/**
 * Given the assets and the example resource, whose waves give only the
 * enemies of normal
 * When the waves are read with the resource started
 * Then every other key of normal is the one of the assets: its curve, its
 * boss, the health of its boss and its look items
 */
TEST(GameAssetPaths, TheExampleResourceKeepsTheRestOfNormal) {
  WavePlan expected = wavesOfTheAssets().planOf(NORMAL_DIFFICULTY);
  const WavePlan laid = wavesWithTheExampleResource().planOf(NORMAL_DIFFICULTY);
  expected.enemies = laid.enemies;

  EXPECT_FALSE(expected.lookItems.empty());
  EXPECT_EQ(laid, expected);
}

/**
 * Given the assets and the example resource, whose waves name neither easy
 * nor hard
 * When the waves are read with the resource started
 * Then easy and hard are those of the assets, look items included
 */
TEST(GameAssetPaths, TheExampleResourceLeavesEasyAndHardAsTheAssetsGiveThem) {
  const WaveCatalog assets = wavesOfTheAssets();
  const WaveCatalog laid = wavesWithTheExampleResource();

  for (const std::string_view difficulty :
       {EASY_DIFFICULTY, CHOSEN_DIFFICULTY}) {
    EXPECT_EQ(laid.planOf(difficulty), assets.planOf(difficulty)) << difficulty;
  }
}

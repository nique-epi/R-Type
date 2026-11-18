#include <gtest/gtest.h>
#include "AssetIds.hpp"
#include "AssetIdsFixtures.hpp"
#include "AssetTestConstants.hpp"
#include "PathText.hpp"
#include "TemporaryFolder.hpp"

using rtype::client::isValidAssetId;
using rtype::client::isWholeFilePath;
using rtype::client::testing::PLAYER_SHIP_ID;
using rtype::logging::genericText;

/**
 * Given an asset id that follows the rule
 * When it is checked
 * Then it is valid
 */
TEST_P(AcceptedAssetId, IsValid) { EXPECT_TRUE(isValidAssetId(GetParam())); }

INSTANTIATE_TEST_SUITE_P(IdsWithinTheRule, AcceptedAssetId,
                         ::testing::Values("a", "0", "ship.png",
                                           "level0/ship9.png",
                                           "sprites/player_ship-2.png",
                                           "sprites/r-typesheet1.gif", "a..b"));

/**
 * Given an asset id that breaks the rule
 * When it is checked
 * Then it is invalid
 */
TEST_P(RejectedAssetId, IsInvalid) { EXPECT_FALSE(isValidAssetId(GetParam())); }

INSTANTIATE_TEST_SUITE_P(
    IdsOutsideTheRule, RejectedAssetId,
    ::testing::Values("", "/sprites/ship.png", "sprites/", "sprites//ship.png",
                      ".", "..", "../ship.png", "sprites/../ship.png",
                      ".hidden", "sprites/.ds_store", "_ship.png", "-ship.png",
                      "Sprites/ship.png", "sprites\\ship.png", "c:/ship.png",
                      "sprites/player ship.png", "sprites/ship`.png",
                      "sprites/ship{.png", "sprites/\xC3\xA9.png"));

/**
 * Given an asset id
 * When it is asked whether it is the whole path of a file
 * Then it is not: it names a file of the assets folder
 */
TEST(WholeFilePath, IsNotAnAssetId) {
  EXPECT_FALSE(isWholeFilePath(PLAYER_SHIP_ID));
}

/**
 * Given the path of a file from the root of the disk
 * When it is asked whether it is the whole path of a file
 * Then it is
 */
TEST(WholeFilePath, IsAPathFromTheRootOfTheDisk) {
  const TemporaryFolder folder;

  EXPECT_TRUE(isWholeFilePath(genericText(folder.path() / PLAYER_SHIP_ID)));
}

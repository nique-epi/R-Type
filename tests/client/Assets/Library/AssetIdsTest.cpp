#include <gtest/gtest.h>
#include "AssetIds.hpp"
#include "AssetIdsFixtures.hpp"

using rtype::client::isValidAssetId;

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

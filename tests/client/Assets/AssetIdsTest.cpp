#include <gtest/gtest.h>
#include <filesystem>
#include <string_view>
#include "AssetIds.hpp"

using rtype::client::genericText;
using rtype::client::isValidAssetId;

namespace {

class AcceptedAssetId : public ::testing::TestWithParam<std::string_view> {};
class RejectedAssetId : public ::testing::TestWithParam<std::string_view> {};

}  // namespace

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
 * Given a path of nested folders
 * When it is turned into text
 * Then its folders are separated by '/', on every platform
 */
TEST(AssetIds, GenericTextSeparatesFoldersWithASlash) {
  const std::filesystem::path nested =
      std::filesystem::path("sprites") / "enemies" / "bydo.png";

  EXPECT_EQ(genericText(nested), "sprites/enemies/bydo.png");
}

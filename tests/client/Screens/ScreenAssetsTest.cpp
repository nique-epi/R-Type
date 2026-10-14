#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>
#include <filesystem>
#include "AssetLibrary.hpp"
#include "AssetLoadingStep.hpp"
#include "EntitySpriteConstants.hpp"
#include "ScreenAssets.hpp"
#include "ScreenConstants.hpp"

using rtype::client::AssetLibrary;
using rtype::client::AssetLoadingStep;
using rtype::client::ENTITY_SHEET_ID;
using rtype::client::gameAssets;
using rtype::client::INTERFACE_FONT_ID;
using rtype::client::loadInterfaceAssets;
using rtype::client::LOGO_ID;
using rtype::client::LOGO_SIZE;
using rtype::client::titleAssets;
using ::testing::Contains;
using ::testing::UnorderedElementsAre;

namespace {

std::filesystem::path repositoryAssets() { return RTYPE_ASSETS_FOLDER; }

}  // namespace

/**
 * Given the assets folder of the repository
 * When the interface assets are loaded at launch
 * Then the font of the screens can be looked up
 */
TEST(ScreenAssets, LaunchLoadsTheInterfaceFont) {
  AssetLibrary library{repositoryAssets()};

  loadInterfaceAssets(library);

  EXPECT_NO_THROW(static_cast<void>(library.font(INTERFACE_FONT_ID)));
}

/**
 * Given the interface assets loaded at launch
 * When the loading step of a game starts
 * Then the font of the screens is still loaded
 */
TEST(ScreenAssets, EnteringAGameKeepsTheInterfaceFont) {
  AssetLibrary library{repositoryAssets()};
  loadInterfaceAssets(library);

  const AssetLoadingStep gameLoading(library, gameAssets());

  EXPECT_NO_THROW(static_cast<void>(library.font(INTERFACE_FONT_ID)));
}

/**
 * Given the assets a game needs
 * When its textures are listed
 * Then they hold the entity sheet the entity table draws from
 */
TEST(ScreenAssets, AGameLoadsTheEntitySheet) {
  EXPECT_THAT(gameAssets().textures, Contains(ENTITY_SHEET_ID));
}

/**
 * Given the assets of the title screen
 * When their textures are listed
 * Then they hold the logo and the entity sheet the squadron is drawn from
 */
TEST(ScreenAssets, TheTitleLoadsItsLogoAndTheEntitySheet) {
  EXPECT_THAT(titleAssets().textures,
              UnorderedElementsAre(LOGO_ID, ENTITY_SHEET_ID));
}

/**
 * Given the logo file of the repository
 * When it is read
 * Then it has the size the title screen lays it out with
 */
TEST(ScreenAssets, TheLogoFileHasTheSizeTheTitleLaysOut) {
  sf::Image logo;

  ASSERT_TRUE(logo.loadFromFile(repositoryAssets() / LOGO_ID));

  EXPECT_EQ(sf::Vector2i(logo.getSize()), LOGO_SIZE);
}

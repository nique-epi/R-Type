#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <filesystem>
#include <string>
#include "AssetLibrary.hpp"
#include "AssetLoadingStep.hpp"
#include "DestinationSpriteConstants.hpp"
#include "EffectConstants.hpp"
#include "EntityAppearances.hpp"
#include "EntitySpriteConstants.hpp"
#include "FlightLogSheetConstants.hpp"
#include "HudSpriteConstants.hpp"
#include "InterfaceConstants.hpp"
#include "LeaderboardSpriteConstants.hpp"
#include "ProfessorSpriteConstants.hpp"
#include "ScreenAssets.hpp"
#include "ScreenAssetsFixtures.hpp"
#include "ScreenAssetsTestConstants.hpp"
#include "ScreenConstants.hpp"
#include "SectorMapSpriteConstants.hpp"
#include "ShipHullSpriteConstants.hpp"
#include "ShipLookSpriteConstants.hpp"
#include "ShopSpriteConstants.hpp"

using rtype::client::AssetLibrary;
using rtype::client::AssetLoadingStep;
using rtype::client::DESTINATION_SHEET_ID;
using rtype::client::EFFECT_SHEET_ID;
using rtype::client::ENTITY_SHEET_ID;
using rtype::client::EntityAppearances;
using rtype::client::FLIGHT_LOG_SHEET_ID;
using rtype::client::gameAssets;
using rtype::client::HUD_SHEET_ID;
using rtype::client::introAssets;
using rtype::client::LEADERBOARD_SHEET_ID;
using rtype::client::loadInterfaceAssets;
using rtype::client::LOGO_ID;
using rtype::client::LOGO_SIZE;
using rtype::client::PLUG_ICON_ID;
using rtype::client::PROFESSOR_SHEET_ID;
using rtype::client::REGULAR_FONT_ID;
using rtype::client::SECTOR_MAP_SHEET_ID;
using rtype::client::SHIP_HULL_SHEET_ID;
using rtype::client::SHIP_LOOK_SHEET_ID;
using rtype::client::SHOP_FRONT_ID;
using rtype::client::SHOP_KEEPER_SHEET_ID;
using rtype::client::SHOP_SCENE_ID;
using rtype::client::SMALL_FONT_ID;
using rtype::client::titleAssets;
using rtype::client::withEntitySheets;
using ::testing::Contains;
using ::testing::IsSupersetOf;
using ::testing::UnorderedElementsAre;

/**
 * Given the assets folder of the repository
 * When the interface assets are loaded at launch
 * Then both fonts of the screens can be looked up
 */
TEST(ScreenAssets, LaunchLoadsTheInterfaceFonts) {
  AssetLibrary library{repositoryAssets()};

  loadInterfaceAssets(library);

  EXPECT_NO_THROW(static_cast<void>(library.font(REGULAR_FONT_ID)));
  EXPECT_NO_THROW(static_cast<void>(library.font(SMALL_FONT_ID)));
}

/**
 * Given the interface assets loaded at launch
 * When the loading step of a game starts
 * Then both fonts of the screens are still loaded
 */
TEST(ScreenAssets, EnteringAGameKeepsTheInterfaceFonts) {
  AssetLibrary library{repositoryAssets()};
  loadInterfaceAssets(library);

  const AssetLoadingStep gameLoading(library, gameAssets());

  EXPECT_NO_THROW(static_cast<void>(library.font(REGULAR_FONT_ID)));
  EXPECT_NO_THROW(static_cast<void>(library.font(SMALL_FONT_ID)));
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
 * Given the assets a game needs
 * When its textures are listed
 * Then they hold the effect sheet the explosions are drawn from
 */
TEST(ScreenAssets, AGameLoadsTheEffectSheet) {
  EXPECT_THAT(gameAssets().textures, Contains(EFFECT_SHEET_ID));
}

/**
 * Given the assets a game needs
 * When its textures are listed
 * Then they hold the HUD sheet the status band is drawn from
 */
TEST(ScreenAssets, AGameLoadsTheHudSheet) {
  EXPECT_THAT(gameAssets().textures, Contains(HUD_SHEET_ID));
}

/**
 * Given the assets a game needs
 * When its textures are listed
 * Then they hold the plug icon of a pilot whose signal is lost
 */
TEST(ScreenAssets, AGameLoadsThePlugIcon) {
  EXPECT_THAT(gameAssets().textures, Contains(PLUG_ICON_ID));
}

/**
 * Given the assets a game needs
 * When its textures are listed
 * Then they hold the flight log sheet, so the end screen opens without
 * loading after a game
 */
TEST(ScreenAssets, AGameLoadsTheFlightLogSheet) {
  EXPECT_THAT(gameAssets().textures, Contains(FLIGHT_LOG_SHEET_ID));
}

/**
 * Given the assets of the title screen
 * When their textures are listed
 * Then they hold the logo and the entity sheet the squadron is drawn from
 */
TEST(ScreenAssets, TheTitleLoadsItsLogoAndTheEntitySheet) {
  EXPECT_THAT(titleAssets().textures, IsSupersetOf({LOGO_ID, ENTITY_SHEET_ID}));
}

/**
 * Given the assets of the title screen
 * When their textures are listed
 * Then they hold the sheet of the leaderboard, since it is pushed over the
 * title and the waiting room without loading
 */
TEST(ScreenAssets, TheTitleLoadsTheSheetOfTheLeaderboard) {
  EXPECT_THAT(titleAssets().textures, IsSupersetOf({LEADERBOARD_SHEET_ID}));
}

/**
 * Given the assets of the title screen
 * When their textures are listed
 * Then they hold the sheet of the items of the ship, since the hangar and the
 * waiting room draw them and are pushed over the title without loading
 */
TEST(ScreenAssets, TheTitleLoadsTheSheetOfTheShipItems) {
  EXPECT_THAT(titleAssets().textures, IsSupersetOf({SHIP_LOOK_SHEET_ID}));
}

/**
 * Given the assets of the title screen
 * When their textures are listed
 * Then they hold the sheets the help of the waiting room draws, since the
 * waiting room is pushed over the title without loading
 */
TEST(ScreenAssets, TheTitleLoadsTheSheetsOfTheHelpOfTheWaitingRoom) {
  EXPECT_THAT(titleAssets().textures,
              IsSupersetOf({HUD_SHEET_ID, PROFESSOR_SHEET_ID}));
}

/**
 * Given the assets of the title screen
 * When their textures are listed
 * Then they hold the sheet of the destination screen, which PLAY pushes over
 * the title without loading
 */
TEST(ScreenAssets, TheTitleLoadsTheSheetOfTheDestinations) {
  EXPECT_THAT(titleAssets().textures, Contains(DESTINATION_SHEET_ID));
}

/**
 * Given the assets of a game
 * When their textures are listed
 * Then they hold the professor sheet, so the help of the waiting room after
 * the game has it
 */
TEST(ScreenAssets, AGameKeepsTheProfessorSheetForTheWaitingRoomAfterIt) {
  EXPECT_THAT(gameAssets().textures, Contains(PROFESSOR_SHEET_ID));
}

/**
 * Given the assets of a game and those of the title screen
 * When their textures are listed
 * Then a game keeps every texture of the title, so a title built after a game
 * opens without a loading screen, which would close what is pushed over it
 */
TEST(ScreenAssets, AGameKeepsEveryTextureOfTheTitle) {
  EXPECT_THAT(gameAssets().textures, IsSupersetOf(titleAssets().textures));
}

/**
 * Given the assets of the intro
 * When their textures are listed
 * Then they hold those of the title, which follows it, the sheet of the map
 * of the servers, and the effect sheet of its explosions
 */
TEST(ScreenAssets, TheIntroLoadsTheTitleAndTheEffectSheet) {
  EXPECT_THAT(introAssets().textures,
              UnorderedElementsAre(
                  LOGO_ID, ENTITY_SHEET_ID, HUD_SHEET_ID, PROFESSOR_SHEET_ID,
                  LEADERBOARD_SHEET_ID, SHIP_LOOK_SHEET_ID, SHIP_HULL_SHEET_ID,
                  SECTOR_MAP_SHEET_ID, SHOP_SCENE_ID, SHOP_KEEPER_SHEET_ID,
                  SHOP_FRONT_ID, DESTINATION_SHEET_ID, EFFECT_SHEET_ID));
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

/**
 * Given the entity sheets shipped in assets/, all drawn from the entity sheet
 * When the sheets of their appearances are added to the assets of a game
 * Then the entity sheet is listed once: a sheet already listed is not added
 * again
 */
TEST(ScreenAssets, AnEntitySheetAlreadyListedIsListedOnce) {
  const EntityAppearances shipped = EntityAppearances::fromFolder(
      repositoryAssets() / SHIPPED_ENTITY_SHEET_FOLDER);

  EXPECT_EQ(std::ranges::count(withEntitySheets(gameAssets(), shipped).textures,
                               ENTITY_SHEET_ID),
            1);
}

/**
 * Given an appearance drawn from a sprite sheet the game does not list
 * When the sheets of the appearances are added to the assets of a game
 * Then that sprite sheet is listed, so the game screen loads it
 */
TEST(ScreenAssets, TheSheetOfANewAppearanceIsLoadedWithTheGame) {
  const EntityAppearances added = appearanceDrawnFrom(NEW_ENTITY_SHEET_ID);

  EXPECT_THAT(withEntitySheets(gameAssets(), added).textures,
              Contains(std::string{NEW_ENTITY_SHEET_ID}));
}

#include <gtest/gtest.h>
#include "ColorMode.hpp"
#include "InterfaceColorConstants.hpp"
#include "PilotShades.hpp"
#include "PilotTestConstants.hpp"
#include "PlayerPalette.hpp"
#include "PlayerPaletteTestConstants.hpp"
#include "PlayerPreferences.hpp"
#include "PlayerPreferencesFixtures.hpp"
#include "PlayerPreferencesTestConstants.hpp"
#include "PlayerSettings.hpp"

using rtype::client::ColorMode;
using rtype::client::PilotShades;
using rtype::client::pilotShadesOf;
using rtype::client::PLAYER_COLORS;
using rtype::client::PlayerPalette;
using rtype::client::playerPaletteOf;
using rtype::client::PlayerSettings;
using rtype::client::testing::BLUISH_GREEN;
using rtype::client::testing::BLUISH_GREEN_SHADES;
using rtype::client::testing::COLOR_BLIND_YELLOW;
using rtype::client::testing::DEBRIS_SHIP_SHADE;
using rtype::client::testing::DEEP_GREEN;
using rtype::client::testing::MAGENTA;
using rtype::client::testing::SHEET_GREEN_SHADES;

/**
 * Given settings with no color mode
 * When the palette is asked for
 * Then it is the palette of the sheets: player 3 stays green
 */
TEST(PlayerPreferences, NoColorModeKeepsTheShadesOfTheSheets) {
  const PlayerSettings settings = settingsFor(ColorMode::Off);

  const PlayerPalette& palette = playerPaletteOf(settings);

  EXPECT_EQ(palette, PLAYER_COLORS);
  EXPECT_EQ(palette.at(THIRD_PILOT_COLOR), SHEET_GREEN_SHADES);
}

/**
 * Given settings for protanopia
 * When the palette is asked for
 * Then player 2 is magenta and player 4 yellow
 */
TEST(PlayerPreferences, ProtanopiaPaintsPlayerTwoMagenta) {
  const PlayerSettings settings = settingsFor(ColorMode::Protanopia);

  const PlayerPalette& palette = playerPaletteOf(settings);

  EXPECT_EQ(palette.at(SECOND_PILOT_COLOR).ship, MAGENTA);
  EXPECT_EQ(palette.back().ship, COLOR_BLIND_YELLOW);
}

/**
 * Given settings for deuteranopia
 * When the palette is asked for
 * Then player 3 is painted in bluish greens
 */
TEST(PlayerPreferences, DeuteranopiaPaintsPlayerThreeBluishGreen) {
  const PlayerSettings settings = settingsFor(ColorMode::Deuteranopia);

  EXPECT_EQ(playerPaletteOf(settings).at(THIRD_PILOT_COLOR),
            BLUISH_GREEN_SHADES);
}

/**
 * Given settings for tritanopia
 * When the palette is asked for
 * Then player 2 is deep green and player 3 keeps the green of the sheets
 */
TEST(PlayerPreferences, TritanopiaPaintsPlayerTwoDeepGreen) {
  const PlayerSettings settings = settingsFor(ColorMode::Tritanopia);

  const PlayerPalette& palette = playerPaletteOf(settings);

  EXPECT_EQ(palette.at(SECOND_PILOT_COLOR).ship, DEEP_GREEN);
  EXPECT_EQ(palette.at(THIRD_PILOT_COLOR), SHEET_GREEN_SHADES);
}

/**
 * Given settings for deuteranopia
 * When the shades of the effects are asked for
 * Then the debris of player 3 go through its bluish green
 */
TEST(PlayerPreferences, TheDebrisFollowThePalette) {
  const PlayerSettings settings = settingsFor(ColorMode::Deuteranopia);

  const PilotShades shades = pilotShadesOf(settings);

  EXPECT_EQ(shades.debris.get().at(THIRD_PILOT_COLOR).at(DEBRIS_SHIP_SHADE),
            BLUISH_GREEN);
}

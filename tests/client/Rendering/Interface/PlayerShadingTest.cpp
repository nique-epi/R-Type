#include <gtest/gtest.h>
#include <cstddef>
#include "InterfaceColorConstants.hpp"
#include "PilotTestConstants.hpp"
#include "PlayerPalette.hpp"
#include "PlayerPaletteConstants.hpp"
#include "PlayerPaletteTestConstants.hpp"
#include "PlayerShading.hpp"
#include "ShadeSwap.hpp"

using rtype::client::DEUTERANOPIA_PLAYER_PALETTE;
using rtype::client::PLAYER_COLORS;
using rtype::client::PlayerPalette;
using rtype::client::PlayerShading;
using rtype::client::PROTANOPIA_PLAYER_PALETTE;
using rtype::client::ShadeSwap;
using rtype::client::shadingOf;
using rtype::client::TRITANOPIA_PLAYER_PALETTE;
using rtype::client::testing::BLUISH_GREEN_SHADES;
using rtype::client::testing::SHEET_GREEN_SHADES;

/**
 * Given the palette of the sheets
 * When each player color is shaded
 * Then none asks for a swap: the frames keep the colors of the sheets
 */
TEST(PlayerShading, TheSheetPaletteSwapsNothing) {
  for (std::size_t color = 0; color < PLAYER_COLORS.size(); ++color) {
    const PlayerShading shading = shadingOf(PLAYER_COLORS, color);

    EXPECT_FALSE(shading.recolor.has_value());
    EXPECT_EQ(shading.shades, PLAYER_COLORS.at(color));
  }
}

/**
 * Given each palette for color blindness
 * When player 1 is shaded
 * Then it asks for no swap: its blue is the blue of the interface
 */
TEST(PlayerShading, PlayerOneKeepsItsBlueInEveryPalette) {
  for (const PlayerPalette* palette :
       {&PROTANOPIA_PLAYER_PALETTE, &DEUTERANOPIA_PLAYER_PALETTE,
        &TRITANOPIA_PLAYER_PALETTE}) {
    EXPECT_FALSE(shadingOf(*palette, FIRST_PILOT_COLOR).recolor.has_value());
  }
}

/**
 * Given the palette for deuteranopia
 * When player 3 is shaded
 * Then its frames swap the greens of the sheet for the bluish greens of the
 * palette
 */
TEST(PlayerShading, AChangedColorSwapsTheSheetShadesForThePalettes) {
  const PlayerShading shading =
      shadingOf(DEUTERANOPIA_PLAYER_PALETTE, THIRD_PILOT_COLOR);

  ASSERT_TRUE(shading.recolor.has_value());
  const ShadeSwap swap = shading.recolor.value_or(ShadeSwap{});
  EXPECT_EQ(swap.from, SHEET_GREEN_SHADES);
  EXPECT_EQ(swap.to, BLUISH_GREEN_SHADES);
  EXPECT_EQ(shading.shades, BLUISH_GREEN_SHADES);
}

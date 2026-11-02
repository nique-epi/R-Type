#pragma once

#include <array>
#include <cstddef>
#include "EntitySpriteConstants.hpp"
#include "SheetRow.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/**
 * @brief Asset id of the sheet of the status band: the lives and the markers
 * of the players, then the icons of the notices of a game.
 *
 * Its sprites are at their native size, SPRITE_SPACING apart, like those of
 * the entity sheet: one pixel of the sheet is one pixel of the interface
 * picture.
 */
constexpr const char* HUD_SHEET_ID = "sprites/hud.png";

/** @brief Lives in the first row: one per player color, in the order of the
 * rows of the entity sheet, then a life already lost. */
constexpr std::size_t LIFE_ROW_LENGTH =
    static_cast<std::size_t>(PLAYER_COLOR_COUNT) + 1;

/** @brief One life: the ship of the first player color, seen small. */
constexpr game::SpriteFrame LIFE_FRAME{
    .left = 0, .top = 0, .width = 16, .height = 8};

constexpr std::array LIFE_ROW =
    framesInRow<LIFE_ROW_LENGTH>(LIFE_FRAME, SPRITE_SPACING);

/** @brief A life already lost: the silhouette of a life, in the shade of the
 * far stars. */
constexpr game::SpriteFrame LOST_LIFE_FRAME = LIFE_ROW.back();

/** @brief The marker of the first player color, a shape that tells the
 * players apart without their color: a triangle, then a square, a circle and
 * a star. */
constexpr game::SpriteFrame MARKER_FRAME{
    .left = 0,
    .top = bottomOf(LIFE_FRAME) + SPRITE_SPACING,
    .width = 7,
    .height = 7};

constexpr std::array MARKER_FRAMES =
    framesInRow<static_cast<std::size_t>(PLAYER_COLOR_COUNT)>(MARKER_FRAME,
                                                              SPRITE_SPACING);

/**
 * @brief Asset id of the unplugged plug shown for a pilot whose signal is
 * lost: "plugs", bold weight, of Phosphor Icons (MIT License), drawn white on
 * a square of 64 pixels, so a tint gives it its color.
 */
constexpr const char* PLUG_ICON_ID = "icons/phosphor/plugs_bold.png";

/**
 * @brief The third row of the sheet, under the markers: the door a pilot
 * left through, beside LEFT THE GAME in the feed of departures, and the bars
 * of a weak signal, the first one lit, beside WAITING FOR THE SERVER. Both
 * are pale and dark blue with an ink outline, so no palette recolors them.
 */
constexpr game::SpriteFrame LEFT_GAME_FRAME{
    .left = 0,
    .top = bottomOf(MARKER_FRAME) + SPRITE_SPACING,
    .width = 9,
    .height = 11};
constexpr game::SpriteFrame WEAK_SIGNAL_FRAME{
    .left = LEFT_GAME_FRAME.left + LEFT_GAME_FRAME.width + SPRITE_SPACING,
    .top = LEFT_GAME_FRAME.top,
    .width = 13,
    .height = 10};

}  // namespace rtype::client

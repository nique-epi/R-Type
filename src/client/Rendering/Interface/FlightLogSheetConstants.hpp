#pragma once

#include <array>
#include "EntitySpriteConstants.hpp"
#include "SheetRow.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/**
 * @brief Asset id of the sheet of the flight log of the end screen: the halves
 * of the book, its label, its shadow, the medal and the cross of QUIT.
 *
 * Its sprites are at their native size, SPRITE_SPACING apart, like those of
 * the HUD sheet: one pixel of the sheet is one pixel of the interface picture.
 */
constexpr const char* FLIGHT_LOG_SHEET_ID = "sprites/flight_log.png";

/** @brief The three halves of the book, side by side: the front cover, the
 * inside of the cover with the left page, the right half with its page. */
constexpr game::SpriteFrame COVER_FRONT_FRAME{
    .left = 0, .top = 0, .width = 208, .height = 206};
constexpr std::array BOOK_HALF_FRAMES =
    framesInRow<3>(COVER_FRONT_FRAME, SPRITE_SPACING);
constexpr game::SpriteFrame COVER_INSIDE_FRAME = BOOK_HALF_FRAMES[1];
constexpr game::SpriteFrame RIGHT_HALF_FRAME = BOOK_HALF_FRAMES[2];

/** @brief Under the halves, from the left: the paper label of the taunt, a
 * tile of the dithered shadow, the medal of the best pilot, and the cross of
 * QUIT, drawn white so a tint gives it its color. */
constexpr game::SpriteFrame TAUNT_LABEL_FRAME{
    .left = 0,
    .top = bottomOf(COVER_FRONT_FRAME) + SPRITE_SPACING,
    .width = 156,
    .height = 50};
constexpr game::SpriteFrame SHADOW_TILE_FRAME{
    .left = TAUNT_LABEL_FRAME.left + TAUNT_LABEL_FRAME.width + SPRITE_SPACING,
    .top = TAUNT_LABEL_FRAME.top,
    .width = 32,
    .height = 32};
constexpr game::SpriteFrame MEDAL_FRAME{
    .left = SHADOW_TILE_FRAME.left + SHADOW_TILE_FRAME.width + SPRITE_SPACING,
    .top = TAUNT_LABEL_FRAME.top,
    .width = 9,
    .height = 10};
constexpr game::SpriteFrame CROSS_FRAME{
    .left = MEDAL_FRAME.left + MEDAL_FRAME.width + SPRITE_SPACING,
    .top = TAUNT_LABEL_FRAME.top,
    .width = 7,
    .height = 7};

}  // namespace rtype::client

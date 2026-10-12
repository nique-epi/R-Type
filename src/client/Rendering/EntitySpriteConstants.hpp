#pragma once

#include "SpriteFrame.hpp"

namespace rtype::client {

/**
 * @brief Asset id of the sprite sheet that holds every entity of a match.
 *
 * Its sprites are at their native size: one pixel of the sheet is one pixel of
 * a PixelCanvas, which covers BACKGROUND_PIXEL_SIZE logical units, like one
 * pixel of the scrolling background.
 */
constexpr const char* ENTITY_SHEET_ID = "sprites/entities.png";

/**
 * @brief Rectangles of the entity sheet, in pixels of the sheet.
 *
 * Each is the first of the two animation frames of its entity. The player ship
 * is the first of the four player colors.
 */
constexpr game::SpriteFrame PLAYER_SHIP_FRAME{
    .left = 0, .top = 0, .width = 32, .height = 16};
constexpr game::SpriteFrame BYDO_FRAME{
    .left = 0, .top = 72, .width = 24, .height = 24};
constexpr game::SpriteFrame PLAYER_MISSILE_FRAME{
    .left = 0, .top = 124, .width = 16, .height = 6};
constexpr game::SpriteFrame ENEMY_MISSILE_FRAME{
    .left = 0, .top = 132, .width = 8, .height = 8};

}  // namespace rtype::client

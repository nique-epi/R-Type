#pragma once

namespace rtype::game {

/** @brief The logical units on each side of one pixel of a sprite: one cell
 * of a collision mask covers SPRITE_PIXEL_UNITS by SPRITE_PIXEL_UNITS units,
 * as the client draws a sprite (BACKGROUND_PIXEL_SIZE of the client). */
constexpr float SPRITE_PIXEL_UNITS = 4.0F;

}  // namespace rtype::game

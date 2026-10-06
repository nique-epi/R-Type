#pragma once

namespace rtype::game {

/**
 * @brief Size of the playfield, in logical units.
 *
 * Every position, velocity and collision box of the game is expressed in this
 * frame, on the server and on the client alike. The origin is the top-left
 * corner of the playfield, x grows to the right and y grows downwards. A
 * logical unit is not a pixel: the client scales the playfield to whatever
 * size its window has.
 *
 * The playfield has the proportions of a 16:9 screen. Its dimensions currently
 * equal the size the client window opens with, in pixels; nothing may rely on
 * that equality.
 */
constexpr float PLAYFIELD_WIDTH = 1920.0F;
constexpr float PLAYFIELD_HEIGHT = 1080.0F;

}  // namespace rtype::game

#pragma once

namespace rtype::game {

/**
 * @brief Size of the playfield, in logical units.
 *
 * Every position, velocity and collision box of the game is expressed in this
 * frame, on the server and on the client alike. The origin is the top-left
 * corner of the playfield, x grows to the right and y grows downwards. A
 * logical unit is not a pixel: the client must scale the playfield to
 * whatever size its window has.
 *
 * These dimensions are provisional. They currently equal the size of the
 * client window in pixels and are meant to be revised once the proportions of
 * the game are decided; nothing may rely on that equality.
 */
constexpr float PLAYFIELD_WIDTH = 800.0F;
constexpr float PLAYFIELD_HEIGHT = 600.0F;

}  // namespace rtype::game

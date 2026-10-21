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
 * The playfield has the proportions of a 16:9 screen. Its dimensions currently
 * equal the size the client window opens with, in pixels; nothing may rely on
 * that equality.
 */
constexpr float PLAYFIELD_WIDTH = 1920.0F;
constexpr float PLAYFIELD_HEIGHT = 1080.0F;

/**
 * @brief Height of the band along the bottom of the playfield that every
 * client covers with the status of the players, in logical units.
 *
 * Nothing of a match is placed there: whatever a player must see stays
 * within PLAY_AREA_HEIGHT of the top edge.
 */
constexpr float STATUS_BAND_HEIGHT = 96.0F;

/** @brief Height of the part of the playfield a match is played in, from its
 * top edge: the playfield less the status band. */
constexpr float PLAY_AREA_HEIGHT = PLAYFIELD_HEIGHT - STATUS_BAND_HEIGHT;

}  // namespace rtype::game

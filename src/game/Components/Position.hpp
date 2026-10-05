#pragma once

namespace rtype::game {

/**
 * @brief Where an entity is, in logical units of the playfield.
 *
 * It is the center of the entity. See PlayfieldConstants.hpp for the origin
 * and the direction of the axes.
 */
struct Position {
  float x;
  float y;
};

}  // namespace rtype::game

#pragma once

#include <SFML/Graphics/Color.hpp>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Component of a speed line: a line one pixel thick on the row of the
 * Position of its entity, that runs from the left edge of the playfield to
 * that Position, its head first, then withdraws behind it. A ship arriving
 * draws four of them.
 */
struct SpeedLine {
  engine::Duration age{};
  engine::Duration life{};
  sf::Color color;
};

}  // namespace rtype::client

#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief The part of the world a frame of the intro shows: each pixel of the
 * world covers zoom × zoom pixels of the canvas, and the pixel of the world at
 * topLeft lands on the top-left corner of the canvas. */
struct CinemaView {
  int zoom = 1;
  sf::Vector2i topLeft;
  bool operator==(const CinemaView&) const = default;
};

}  // namespace rtype::client

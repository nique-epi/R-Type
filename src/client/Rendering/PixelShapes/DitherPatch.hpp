#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace rtype::client {

/** @brief A rectangle dithered with one color: the share of its pixels lit
 * goes from topShare on its first row to bottomShare on its last, 0 to 1. */
struct DitherPatch {
  sf::IntRect area;
  sf::Color color;
  float topShare{};
  float bottomShare{};
};

}  // namespace rtype::client

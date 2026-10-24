#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace rtype::client {

/** @brief A texture laid smoothed over the picture: its smoothed copy, the
 * area it covers, in pixels of the picture, and its tint. */
struct SmoothImage {
  const sf::Texture* texture{};
  sf::IntRect area;
  sf::Color tint;
};

}  // namespace rtype::client

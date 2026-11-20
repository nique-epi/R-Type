#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/**
 * @brief A beam of light swept across a room of the casino: a thin triangle
 * from its origin, pointing at its angle, in radians from straight up and
 * clockwise, as long as its length and halfSpread radians wide on each side,
 * painted in its color, whose alpha is how strong it shines.
 */
struct LightBeam {
  sf::Vector2f origin;
  float angle{};
  float length{};
  float halfSpread{};
  sf::Color color;
};

}  // namespace rtype::client

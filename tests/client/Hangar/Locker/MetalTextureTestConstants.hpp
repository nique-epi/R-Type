#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <cstdint>

namespace rtype::client::testing {

/** @brief A plate six pixels wide, the smallest the scratches may start in,
 * two shades, and a seed. */
constexpr sf::IntRect SIX_BY_SIX{{10, 20}, {6, 6}};
constexpr sf::IntRect LARGE_PLATE{{0, 0}, {138, 187}};
constexpr sf::Color LIGHT_SHADE(200, 200, 200);
constexpr sf::Color DARK_SHADE(20, 20, 20);
constexpr std::uint32_t ANY_SEED = 7;

}  // namespace rtype::client::testing

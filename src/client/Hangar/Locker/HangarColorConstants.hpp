#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/** @brief The deepest black of the hangar: the gaps between the panels of
 * the bay, the shade of the rivets, the shaded sides of the niches. */
constexpr sf::Color HANGAR_SHADOW(6, 7, 15);

/** @brief Steel in the dark: the panels of the bay and the back of the
 * niches, light enough for the INK outline of an item to read on it. */
constexpr sf::Color STEEL_DARK(21, 26, 54);

}  // namespace rtype::client

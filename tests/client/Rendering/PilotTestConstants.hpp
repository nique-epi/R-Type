#pragma once

#include <SFML/Graphics/Color.hpp>

/** @brief The color of the first pilot, whose blue no palette changes. */
constexpr int FIRST_PILOT_COLOR = 0;

/** @brief The color of the second pilot, and the main and light shades of its
 * ship, read from the second row of entities.png: what its debris, its
 * outline and the figures of its gains are drawn in. */
constexpr int SECOND_PILOT_COLOR = 1;
constexpr sf::Color PINK_OF_THE_SECOND_PILOT(217, 74, 140);
constexpr sf::Color PINK_LIGHT_OF_THE_SECOND_PILOT(255, 163, 207);

/** @brief The color of the third pilot, whose shades the palettes for
 * protanopia and deuteranopia change. */
constexpr int THIRD_PILOT_COLOR = 2;

/** @brief The number of the third player, who flies the third pilot color. */
constexpr int THIRD_PLAYER_NUMBER = 3;

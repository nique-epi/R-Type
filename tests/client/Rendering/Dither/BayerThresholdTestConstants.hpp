#pragma once

#include <SFML/System/Vector2.hpp>

/** @brief The pixel at the origin, where the matrix holds its lowest level,
 * and its threshold: half a level up, out of sixteen. */
constexpr sf::Vector2i CORNER_OF_THE_MATRIX(0, 0);
constexpr double LOWEST_THRESHOLD = 0.5 / 16.0;

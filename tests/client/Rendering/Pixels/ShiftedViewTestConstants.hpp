#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

/** @brief The area a view of the canvas shows: 480 × 270 pixels. */
constexpr sf::FloatRect CANVAS_AREA({0.0F, 0.0F}, {480.0F, 270.0F});

/** @brief A shake of 2 pixels right and 1 up. */
constexpr sf::Vector2i SHAKE(2, -1);

/** @brief A pixel drawn through the views. */
constexpr sf::Vector2f A_PIXEL(100.0F, 50.0F);

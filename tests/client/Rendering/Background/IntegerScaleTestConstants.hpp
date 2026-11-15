#pragma once

#include <SFML/System/Vector2.hpp>

/** @brief Playfields on screen, in pixels, against the background of 480 x
 * 270 pixels. */
constexpr sf::Vector2i THREE_AND_A_THIRD_BACKGROUNDS(1600, 900);
constexpr sf::Vector2i TWICE_THE_BACKGROUND(960, 540);
constexpr sf::Vector2i JUST_UNDER_TWICE_THE_BACKGROUND(959, 539);
constexpr sf::Vector2i FOUR_WIDE_THREE_HIGH(1920, 810);
constexpr sf::Vector2i NO_SIZE(0, 0);

/** @brief Whole factors the background is enlarged by. */
constexpr unsigned int ONCE = 1;
constexpr unsigned int TWICE = 2;
constexpr unsigned int THREE_TIMES = 3;
constexpr unsigned int FOUR_TIMES = 4;

/** @brief Densities of a canvas: the sky's, and a canvas that writes text,
 * whose texture gives each of its pixels two on each axis. */
constexpr unsigned int SKY_DENSITY = 1;
constexpr unsigned int TEXT_CANVAS_DENSITY = 2;

/** @brief How much the texture of a text canvas is enlarged at 1920 x 1080
 * (factor 4) and 1600 x 900 (factor 3): half the factor of the sky. */
constexpr float TEXT_CANVAS_AT_FOUR_TIMES = 2.0F;
constexpr float TEXT_CANVAS_AT_THREE_TIMES = 1.5F;
constexpr float SKY_AT_THREE_TIMES = 3.0F;

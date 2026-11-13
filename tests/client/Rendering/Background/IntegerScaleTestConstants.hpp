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

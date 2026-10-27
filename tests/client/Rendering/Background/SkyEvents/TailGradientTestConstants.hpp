#pragma once

#include <SFML/Graphics/Color.hpp>
#include "TailGradient.hpp"

/** @brief The colors of the four stops of GRADIENT, head first. */
constexpr sf::Color FIRST_STOP_WHITE(250, 250, 250);
constexpr sf::Color SECOND_STOP_PERIWINKLE(150, 150, 250);
constexpr sf::Color THIRD_STOP_BLUE(80, 80, 200);
constexpr sf::Color LAST_STOP_NAVY(20, 20, 90);

/** @brief A tail gradient of four stops, ending at 0.2, 0.5, 0.8 and 1. */
constexpr rtype::client::TailGradient GRADIENT{
    rtype::client::GradientStop{.end = 0.2F, .color = FIRST_STOP_WHITE},
    rtype::client::GradientStop{.end = 0.5F, .color = SECOND_STOP_PERIWINKLE},
    rtype::client::GradientStop{.end = 0.8F, .color = THIRD_STOP_BLUE},
    rtype::client::GradientStop{.end = 1.0F, .color = LAST_STOP_NAVY},
};

/** @brief Places along a tail: its head, exactly the end of the second stop,
 * and past the end of every stop. */
constexpr float AT_THE_HEAD = 0.0F;
constexpr float ON_A_STOP_END = 0.5F;
constexpr float PAST_EVERY_STOP = 1.5F;

#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

/** @brief A call written as the end screen writes its own: a letter every
 * 20 ms, a glow of 2 pixels (12 writes around each line), shown two steps of
 * 300 ms then hidden one. */
constexpr std::string_view CALL_WORDS = "PRESS ENTER";
constexpr sf::Vector2i CALL_PLACE(10, 20);
constexpr rtype::engine::Duration BEFORE_THE_FIRST_LETTER =
    std::chrono::milliseconds(-1);
constexpr rtype::engine::Duration TWO_LETTER_TIMES =
    std::chrono::milliseconds(40);
constexpr std::string_view FIRST_THREE_LETTERS = "PRE";
constexpr rtype::engine::Duration THIRD_BLINK_STEP =
    std::chrono::milliseconds(600);
constexpr std::size_t GLOW_WRITES = 12;
constexpr sf::Vector2i ONE_PIXEL_LOWER(0, 1);
constexpr sf::Color GOLD(255, 197, 49);
constexpr sf::Color DARK_ORANGE(122, 74, 16);
constexpr sf::Color PRESSED_WHITE(238, 241, 255);

#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <chrono>
#include <string_view>
#include "TimeConstants.hpp"

/** @brief The card of the design, F V3: the 75 % gray bar from column 0, 69
 * wide and 180 tall; the white of the bottom row from column 86 to 170,
 * under the bars and castellations (202 rows); the words of the box and of
 * the call. */
constexpr sf::IntRect GRAY_BAR({0, 0}, {69, 180});
constexpr sf::IntRect BOTTOM_WHITE({86, 202}, {85, 68});
constexpr sf::Color SEVENTY_FIVE_GRAY(192, 192, 192);
constexpr sf::Color FULL_WHITE(255, 255, 255);
constexpr std::string_view NO_SIGNAL_WORDS = "NO SIGNAL";
constexpr std::string_view SILENT_WORDS = "THE SERVER STOPPED ANSWERING.";
constexpr std::string_view CLOSED_WORDS = "THE SERVER SHUT DOWN.";
constexpr std::string_view RECONNECT_WORDS = "PRESS ENTER TO RECONNECT";
constexpr rtype::engine::Duration CALL_TYPED =
    std::chrono::milliseconds(200 + (24 * 20));

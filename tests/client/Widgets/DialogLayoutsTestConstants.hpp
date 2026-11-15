#pragma once

#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "TextMetrics.hpp"

namespace rtype::client::testing {

/** @brief The question of the quit dialog, 30 letters: 480 pixels at the
 * largest size, more than the 400 a dialog of 432 leaves inside its sides of
 * 16, so it wraps after its 24th letter, "TO", 384 pixels. */
constexpr std::string_view QUIT_HEADING = "CONFIRM";
constexpr std::string_view QUIT_MESSAGE = "ARE YOU SURE YOU WANT TO QUIT?";
constexpr std::string_view QUIT_MESSAGE_FIRST_LINE = "ARE YOU SURE YOU WANT TO";
constexpr std::string_view QUIT_MESSAGE_SECOND_LINE = "QUIT?";
constexpr int WIDEST_DIALOG = 432;

/** @brief The dialog as drawn at the size the fonts are drawn at. */
constexpr sf::Vector2i DRAWN_DIALOG(272, 96);

/** @brief The text sizes. */
constexpr TextMetrics NORMAL_TEXT{};
constexpr TextMetrics LARGEST_TEXT{.halfPixelsPerFontPixel = 4};

}  // namespace rtype::client::testing

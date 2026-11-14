#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <cstdint>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief The key of a first launch that opens the chat, the letter it types,
 * and the row the chat ends on in these tests: the waiting room's. */
constexpr sf::Keyboard::Scancode CHAT_KEY = sf::Keyboard::Scan::T;
constexpr char32_t CHAT_KEY_LETTER = U't';
constexpr int CHAT_TEST_BOTTOM = 246;

/** @brief The letters a line of the chat holds at the largest text size:
 * 480 less two margins of 24, the padding of the field, 4 on each side, and
 * its caret, 1, leaves 423 pixels, 26 letters of 16. */
constexpr int LETTERS_AT_THE_LARGEST_SIZE = 26;

/** @brief A pilot who writes, and what the tests make it write. */
constexpr std::uint8_t NOVA_NUMBER = 2;
constexpr std::string_view NOVA = "NOVA";
constexpr std::string_view NOVA_HEAD = "NOVA: ";
constexpr std::string_view SHORT_LINE = "gg";
constexpr std::string_view LONG_LINE = "go low now please";
constexpr std::string_view LONG_LINE_FIRST = "go";
constexpr std::string_view LONG_LINE_SECOND = "low now";
constexpr std::string_view LONG_LINE_THIRD = "please";
constexpr std::string_view SPACES = "   ";

/** @brief Ages of a line at the default eight seconds: half through its
 * fade, and gone; the alpha half through the fade, 255 / 2 rounded down. */
constexpr engine::Duration HALF_FADED = std::chrono::milliseconds(7500);
constexpr engine::Duration GONE = std::chrono::seconds(8);
constexpr std::uint8_t HALF_ALPHA = 127;

/** @brief Settings the tests choose: the narrowest line, a background at
 * half, the fewest open lines; and the alpha of the half background,
 * 255 * 50 / 100 rounded down. */
constexpr int NARROW_LETTERS = 12;
constexpr int HALF_BACKGROUND = 50;
constexpr std::uint8_t HALF_BACKGROUND_ALPHA = 127;
constexpr int FEW_OPEN_LINES = 4;

/** @brief Numbered lines: four, one more than a closed chat shows; ten, and
 * what the open chat shows of them before and after one PGUP. */
constexpr int FOUR_LINES = 4;
constexpr int OLDEST_SHOWN_OF_FOUR = 2;
constexpr int NUMBERED_LINES = 10;
constexpr std::string_view OLDEST_OF_FOUR = "line 1";
constexpr std::string_view NEWEST_SHOWN_FIRST = "line 7";
constexpr std::string_view SCROLLED_SHOWN_FIRST = "line 3";
constexpr std::string_view SCROLLED_HIDDEN = "line 7";
constexpr std::string_view MORE_BEFORE_SCROLL = "PGUP: 6 MORE LINES";
constexpr std::string_view MORE_AFTER_SCROLL = "PGUP: 2 MORE LINES";
constexpr std::string_view LINE_PREFIX = "line ";

}  // namespace rtype::client::testing

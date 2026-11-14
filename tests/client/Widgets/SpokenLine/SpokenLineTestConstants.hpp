#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A voice of two sounds, played in turn. */
constexpr std::array<const char*, 2> TEST_VOICE{"sounds/test/voice_a.ogg",
                                                "sounds/test/voice_b.ogg"};

/** @brief A bubble wide enough for the lines of these tests on one line. */
constexpr int TEST_BUBBLE_WIDTH = 300;
constexpr sf::Vector2i TEST_BUBBLE_BOTTOM_LEFT{20, 100};
/** @brief The tail, 10 pixels from the left of the bubble: its first row,
 * 3 pixels on each side of its middle, is 7 pixels wide. */
constexpr int TEST_TAIL = 10;
constexpr sf::Vector2i TEST_TAIL_FIRST_ROW_LEFT{27, 100};
constexpr int TEST_TAIL_FIRST_ROW_WIDTH = 7;

/** @brief Two words of 6 then 5 letters: the second starts at the seventh
 * letter, written 6 letter times (180 ms) after the first; the whole line
 * takes 11 letter times (330 ms) to write, then lingers 3 s. */
constexpr std::string_view GREETING_LINE = "PRIVET PILOT";
constexpr std::array<std::string_view, 2> GREETING_LINE_WORDS{"PRIVET",
                                                              "PILOT"};
constexpr engine::Duration SECOND_WORD_STARTS = std::chrono::milliseconds(180);
constexpr engine::Duration GREETING_LINE_SHOWN =
    std::chrono::milliseconds(3330);
constexpr engine::Duration TWO_LETTER_TIMES = std::chrono::milliseconds(60);

/** @brief Other lines, said after or instead of the greeting. */
constexpr std::string_view RADIO_LINE = "RADIO";
constexpr std::string_view BOTTLES_LINE = "HANDS OFF";

/** @brief The step a test walks time in, shorter than a letter time. */
constexpr engine::Duration SHORT_FRAME = std::chrono::milliseconds(10);

}  // namespace rtype::client::testing

#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "SpokenLineConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A voice of two sounds, played in turn. */
constexpr std::array<const char*, 2> TEST_VOICE{"sounds/test/voice_a.ogg",
                                                "sounds/test/voice_b.ogg"};

/** @brief A bubble wide enough for the lines of these tests on one line. */
constexpr int TEST_BUBBLE_WIDTH = 300;
constexpr sf::Vector2i TEST_BUBBLE_BOTTOM_LEFT{20, 100};
/** @brief The tail, 10 pixels from the left of the bubble: its first row,
 * under the bubble, SPOKEN_TAIL_HALF_WIDTH pixels on each side of its
 * middle. */
constexpr int TEST_TAIL = 10;
constexpr sf::Vector2i TEST_TAIL_FIRST_ROW_LEFT =
    TEST_BUBBLE_BOTTOM_LEFT +
    sf::Vector2i(TEST_TAIL - SPOKEN_TAIL_HALF_WIDTH, 0);
constexpr int TEST_TAIL_FIRST_ROW_WIDTH = (2 * SPOKEN_TAIL_HALF_WIDTH) + 1;

/** @brief Two words of 6 then 5 letters: the second starts at the seventh
 * letter, written 6 letter times after the first; the whole line takes 11
 * letter times to write, then lingers SPOKEN_LINGER_TIME. */
constexpr std::string_view GREETING_LINE = "PRIVET PILOT";
constexpr std::array<std::string_view, 2> GREETING_LINE_WORDS{"PRIVET",
                                                              "PILOT"};
constexpr int GREETING_FIRST_WORD_LETTERS = 6;
constexpr int GREETING_LINE_LETTERS = 11;
constexpr engine::Duration SECOND_WORD_STARTS =
    SPOKEN_LETTER_TIME * GREETING_FIRST_WORD_LETTERS;
constexpr engine::Duration GREETING_LINE_SHOWN =
    (SPOKEN_LETTER_TIME * GREETING_LINE_LETTERS) + SPOKEN_LINGER_TIME;
constexpr engine::Duration TWO_LETTER_TIMES = SPOKEN_LETTER_TIME * 2;

/** @brief Other lines, said after or instead of the greeting. */
constexpr std::string_view RADIO_LINE = "RADIO";
constexpr std::string_view BOTTLES_LINE = "HANDS OFF";

/** @brief The step a test walks time in, shorter than a letter time. */
constexpr engine::Duration SHORT_FRAME = std::chrono::milliseconds(10);

}  // namespace rtype::client::testing

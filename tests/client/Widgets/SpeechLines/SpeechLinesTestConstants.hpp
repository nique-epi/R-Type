#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <string_view>

namespace rtype::client::testing {

/** @brief A bubble wide enough for any sentence of these tests on one
 * line. */
constexpr int WIDE_BUBBLE_TEXT = 400;

/** @brief Two plain words, the first six letters long. */
constexpr std::string_view GREETING = "PRIVET PILOT";
constexpr std::string_view GREETING_FIRST_WORD = "PRIVET";
constexpr std::string_view GREETING_SECOND_WORD = "PILOT";
constexpr std::size_t GREETING_FIRST_WORD_LETTERS = GREETING_FIRST_WORD.size();

/** @brief A sentence of five letters, then a keycap, its sixth letter. */
constexpr std::string_view PRESS_KEY_SENTENCE = "PRESS {R}";
constexpr std::size_t PRESS_KEY_SENTENCE_LETTERS = 6;

/** @brief A sentence ending in a keycap whose token a key name gives. */
constexpr std::string_view SHOOT_SENTENCE = "PRESS {SHOOT}.";
constexpr std::string_view SHOOT_TOKEN = "SHOOT";
constexpr std::string_view SHOOT_KEY = "SPACE";
/** @brief PRESS, the keycap, then the dot. */
constexpr std::size_t SHOOT_SENTENCE_LETTERS = 7;

/** @brief A keycap no key name gives, and a highlighted word. */
constexpr std::string_view RADIO_SENTENCE = "*RADIO*! {R}";
constexpr std::string_view RADIO_WORD = "RADIO";
constexpr std::string_view RADIO_KEY = "R";

constexpr sf::Vector2i SPEECH_TOP_LEFT{20, 30};

}  // namespace rtype::client::testing

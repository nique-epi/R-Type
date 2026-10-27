#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"

namespace rtype::client::testing {

/** @brief A text of five letters typed one every 50 ms. */
constexpr rtype::engine::Duration LETTER_TIME = std::chrono::milliseconds(50);
constexpr std::size_t FIVE_LETTERS = 5;
constexpr rtype::engine::Duration ONE_NANOSECOND_EARLY = -ONE_NANOSECOND;
constexpr rtype::engine::Duration TWO_LETTER_TIMES = LETTER_TIME * 2;
constexpr std::size_t THREE_LETTERS = 3;
constexpr rtype::engine::Duration LONG_AFTER = std::chrono::seconds(5);

/** @brief ÉLODIE in UTF-8: É takes two bytes, the text six letters. */
constexpr std::string_view ACCENTED_NAME = "\xC3\x89LODIE";
constexpr std::size_t ACCENTED_NAME_LETTERS = 6;
constexpr std::size_t TWO_LETTERS = 2;
constexpr std::string_view ACCENTED_FIRST_TWO = "\xC3\x89L";

}  // namespace rtype::client::testing

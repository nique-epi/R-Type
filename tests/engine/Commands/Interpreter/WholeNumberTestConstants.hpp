#pragma once

#include <cstdint>
#include <string_view>

/** @brief A number and its text, as a command line holds it. */
constexpr std::string_view FORTY_TWO_TEXT = "42";
constexpr std::int64_t FORTY_TWO = 42;

/** @brief A number followed by a letter: not a whole number. */
constexpr std::string_view NUMBER_WITH_A_LETTER_TEXT = "4x";

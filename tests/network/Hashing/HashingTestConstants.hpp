#pragma once

#include <string_view>

/** @brief A secret a program holds, and what a caller may present for it. */
constexpr std::string_view HELD_SECRET = "c0ffee-held-by-the-program";
constexpr std::string_view SAME_LENGTH_GUESS = "c0ffee-held-by-the-progran";
constexpr std::string_view START_OF_THE_SECRET = "c0ffee-held";
constexpr std::string_view NO_SECRET;

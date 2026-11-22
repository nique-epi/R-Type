#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

/** @brief When the banner writes its last letter, the first at 1 s and one
 * more every 50 ms, and when it is gone. */
constexpr rtype::engine::Duration ALL_LETTERS_WRITTEN =
    std::chrono::milliseconds(1600);
constexpr rtype::engine::Duration BANNER_END = std::chrono::milliseconds(2650);

/** @brief The letters of MISSION START, the space included. */
constexpr std::size_t LETTERS = 13;

/** @brief The first two letters of the French banner, the second an accented
 * letter of two bytes, typed whole. */
constexpr std::size_t TWO_LETTERS = 2;
constexpr std::string_view FRENCH_FIRST_TWO_LETTERS = "DÉ";

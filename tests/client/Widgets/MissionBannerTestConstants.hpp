#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

/** @brief When the banner writes its last letter, the first at 1 s and one
 * more every 50 ms, and when it is gone. */
constexpr rtype::engine::Duration ALL_LETTERS_WRITTEN =
    std::chrono::milliseconds(1600);
constexpr rtype::engine::Duration BANNER_END = std::chrono::milliseconds(2650);

/** @brief The letters of MISSION START, the space included. */
constexpr std::size_t LETTERS = 13;

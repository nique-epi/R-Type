#pragma once

#include <chrono>
#include "TimeConstants.hpp"

/** @brief Two and a half frames of 100 ms, played in one step or in small
 * steps of 10 ms. */
constexpr rtype::engine::Duration TWO_AND_A_HALF_FRAMES =
    std::chrono::milliseconds(250);
constexpr rtype::engine::Duration SMALL_STEP = std::chrono::milliseconds(10);

/** @brief How many cycles a loop is played to show it never ends. */
constexpr int MANY_CYCLES = 10;

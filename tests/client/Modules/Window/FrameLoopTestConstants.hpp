#pragma once

#include <array>
#include <chrono>
#include <string_view>
#include "TimeConstants.hpp"

/** @brief The one stage of the loops the tests run, and the module that adds
 * the work to it. */
constexpr std::string_view TESTED_STAGE = "update";
constexpr std::string_view PACING_MODULE_NAME = "pacing";

/** @brief How long the passes of a test last, by the simulated clock: three
 * different times, as three frames of a real loop. */
constexpr rtype::engine::Duration FIRST_PASS_DURATION =
    std::chrono::milliseconds{16};
constexpr rtype::engine::Duration SECOND_PASS_DURATION =
    std::chrono::milliseconds{17};
constexpr rtype::engine::Duration THIRD_PASS_DURATION =
    std::chrono::milliseconds{15};

/** @brief The durations of a loop of three passes, then of a loop of one. */
constexpr std::array<rtype::engine::Duration, 3> THREE_PASSES{
    FIRST_PASS_DURATION, SECOND_PASS_DURATION, THIRD_PASS_DURATION};
constexpr std::array<rtype::engine::Duration, 1> ONE_PASS{FIRST_PASS_DURATION};

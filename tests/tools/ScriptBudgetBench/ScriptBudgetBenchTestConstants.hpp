#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "BenchPopulation.hpp"
#include "TickSample.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools::testing {

constexpr engine::Duration ONE_MILLISECOND = std::chrono::milliseconds{1};
constexpr engine::Duration TWO_MILLISECONDS = std::chrono::milliseconds{2};
constexpr engine::Duration THREE_MILLISECONDS = std::chrono::milliseconds{3};
constexpr engine::Duration FOUR_MILLISECONDS = std::chrono::milliseconds{4};

constexpr std::array<TickSample, 4> FOUR_TICKS_OUT_OF_ORDER{
    TickSample{.whole = FOUR_MILLISECONDS, .scripts = THREE_MILLISECONDS},
    TickSample{.whole = ONE_MILLISECOND, .scripts = ONE_MILLISECOND},
    TickSample{.whole = THREE_MILLISECONDS, .scripts = ONE_MILLISECOND},
    TickSample{.whole = TWO_MILLISECONDS, .scripts = ONE_MILLISECOND}};
constexpr double SHARE_OF_FOUR_TICKS = 0.6;
constexpr double SHARE_TOLERANCE = 1e-9;

constexpr std::size_t FEW_SCRIPTED_BODIES = 20;
constexpr std::size_t FEW_BULLETS = 50;
constexpr BenchPopulation FEW_BODIES{.scriptedBodies = FEW_SCRIPTED_BODIES,
                                     .bullets = FEW_BULLETS};
/** @brief More ticks than a bullet needs to cross the playfield: 1920 units at
 * 360 units per second are 320 ticks. */
constexpr std::size_t TICKS_TO_CROSS_THE_PLAYFIELD = 330;

constexpr std::string_view SEVEN_BODIES = "7";
constexpr std::size_t SEVEN = 7;
constexpr std::string_view NO_TICK = "0";
constexpr std::string_view NOT_A_NUMBER = "many";

}  // namespace rtype::tools::testing

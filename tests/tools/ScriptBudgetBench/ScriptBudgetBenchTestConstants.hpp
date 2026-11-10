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
    TickSample{.whole = FOUR_MILLISECONDS,
               .scripts = THREE_MILLISECONDS,
               .collisions = ONE_MILLISECOND,
               .collisionsFound = 3},
    TickSample{.whole = ONE_MILLISECOND,
               .scripts = ONE_MILLISECOND,
               .collisions = engine::Duration::zero(),
               .collisionsFound = 1},
    TickSample{.whole = THREE_MILLISECONDS,
               .scripts = ONE_MILLISECOND,
               .collisions = ONE_MILLISECOND,
               .collisionsFound = 0},
    TickSample{.whole = TWO_MILLISECONDS,
               .scripts = ONE_MILLISECOND,
               .collisions = engine::Duration::zero(),
               .collisionsFound = 2}};
constexpr double SHARE_OF_FOUR_TICKS = 0.6;
/** @brief 2 ms of collisions in the 10 ms of FOUR_TICKS_OUT_OF_ORDER. */
constexpr double COLLISION_SHARE_OF_FOUR_TICKS = 0.2;
/** @brief 3 + 1 + 0 + 2 collisions in FOUR_TICKS_OUT_OF_ORDER. */
constexpr std::size_t COLLISIONS_OF_FOUR_TICKS = 6;
constexpr double SHARE_TOLERANCE = 1e-9;

constexpr std::size_t FEW_SCRIPTED_BODIES = 20;
constexpr std::size_t FEW_BULLETS = 50;
constexpr BenchPopulation FEW_BODIES{.scriptedBodies = FEW_SCRIPTED_BODIES,
                                     .bullets = FEW_BULLETS};
/** @brief FEW_BODIES, their script waiting a tick at a time. */
constexpr BenchPopulation FEW_WAITING_BODIES{
    .scriptedBodies = FEW_SCRIPTED_BODIES,
    .bullets = FEW_BULLETS,
    .waiting = true};
/** @brief More ticks than a bullet needs to cross the playfield: 1920 units at
 * 360 units per second are 320 ticks. */
constexpr std::size_t TICKS_TO_CROSS_THE_PLAYFIELD = 330;

/** @brief The cell sides the issue asks the bench to compare. */
constexpr std::array<float, 3> MEASURED_CELL_SIDES{60.0F, 120.0F, 240.0F};
/**
 * @brief The most bullets the four ships can touch when the bullets touch no
 * other bullet: a ship of 128 by 64 and a bullet of 32 meet within 96 units
 * of height, three rows of 33.75, and within 160 units of width, three
 * bullets 61 units apart; 4 ships of 3 by 3 bullets make 36.
 */
constexpr std::size_t MOST_BULLETS_ON_THE_SHIPS = 36;
constexpr std::string_view SIXTY_UNITS = "60";
constexpr float SIXTY = 60.0F;

constexpr std::string_view SEVEN_BODIES = "7";
constexpr std::size_t SEVEN = 7;
constexpr std::string_view NO_TICK = "0";
constexpr std::string_view NOT_A_NUMBER = "many";

}  // namespace rtype::tools::testing

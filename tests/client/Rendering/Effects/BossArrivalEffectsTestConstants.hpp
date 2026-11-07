#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "Position.hpp"
#include "TimeConstants.hpp"

/**
 * @brief The arrival the design sets, from the Arriving status: the camera
 * rumbles at a trauma of 0.41 from 2.4 s to 3.8 s; the boss walks in from
 * 4.4 s for 1.8 s, a step every 450 ms, four in all, each a trauma of 0.58.
 */
constexpr rtype::engine::Duration ARRIVAL_STARTS =
    rtype::engine::Duration::zero();
constexpr rtype::engine::Duration RUMBLE_STARTS =
    std::chrono::milliseconds(2400);
constexpr rtype::engine::Duration RUMBLE_STOPS =
    std::chrono::milliseconds(3800);
constexpr float RUMBLE_TRAUMA = 0.41F;
constexpr rtype::engine::Duration FIRST_STEP = std::chrono::milliseconds(4400);
constexpr rtype::engine::Duration SECOND_STEP = std::chrono::milliseconds(4850);
constexpr rtype::engine::Duration LONG_AFTER_THE_ENTRY =
    std::chrono::seconds(10);
constexpr int STEPS_OF_THE_ENTRY = 4;
constexpr float STEP_TRAUMA = 0.58F;
constexpr float TRAUMA_TOLERANCE = 1e-4F;

/** @brief The boss of the roar tests, and where its mouth is from its
 * center: (33, 75) in its 128-pixel frame, (-31, 11) from the center of the
 * frame, four logical units a pixel. */
constexpr std::uint32_t ROARING_BOSS_ID = 41;
constexpr std::uint32_t UNKNOWN_BOSS_ID = 99;
constexpr rtype::game::Position ROARING_BOSS_CENTER{.x = 1616.0F, .y = 492.0F};
constexpr rtype::game::Position ROARING_BOSS_MOUTH{.x = 1492.0F, .y = 536.0F};
constexpr std::size_t SHOCKWAVES_OF_THE_ROAR = 2;

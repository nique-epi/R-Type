#pragma once

#include <chrono>
#include <cstdint>
#include "TimeConstants.hpp"

/** @brief Times left of a transition of the level: before the ships fly out
 * in a transition longer than 3 s, as they start flying out, while they are
 * away, as they start flying back, one millisecond before its end, and none. */
constexpr rtype::engine::Duration BEFORE_THE_SHIPS_FLY_OUT =
    std::chrono::milliseconds(4000);
constexpr rtype::engine::Duration AS_THE_SHIPS_FLY_OUT =
    std::chrono::milliseconds(3000);
constexpr rtype::engine::Duration WHILE_THE_SHIPS_ARE_AWAY =
    std::chrono::milliseconds(1500);
constexpr rtype::engine::Duration AS_THE_SHIPS_FLY_BACK =
    std::chrono::milliseconds(1000);
constexpr rtype::engine::Duration A_MILLISECOND_LEFT =
    std::chrono::milliseconds(1);
constexpr rtype::engine::Duration NO_TIME_LEFT =
    rtype::engine::Duration::zero();

/** @brief Past the right edge of the playfield (1920) by half a ship (64), so
 * a ship at column 0 is wholly out; and the column of the start of a ship
 * (192) with the edge it must be beyond as it starts flying back, half a ship
 * left of 0. */
constexpr float BEYOND_THE_RIGHT_EDGE = 1984.0F;
constexpr float COLUMN_OF_A_SHIP_START = 192.0F;
constexpr float BEYOND_THE_LEFT_EDGE = -64.0F;
constexpr float SHIFT_TOLERANCE = 0.5F;

/** @brief The time left a LevelStatus tells, 2500 ms, and more than that. */
constexpr std::uint16_t TOLD_TRANSITION_LEFT = 2500;
constexpr rtype::engine::Duration LONGER_THAN_THE_TOLD_TRANSITION =
    std::chrono::milliseconds(3000);
constexpr rtype::engine::Duration TOLD_TRANSITION_DURATION =
    std::chrono::milliseconds(TOLD_TRANSITION_LEFT);
constexpr std::uint16_t NO_TRANSITION_TOLD = 0;

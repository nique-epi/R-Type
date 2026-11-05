#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/** @brief The settings of the shipped Reaper: reach 160, align 16, a rest of
 * 900 ms, a signal of 400 ms, a strike of 140 ms, a recover of 600 ms. */
constexpr std::array<std::int64_t, 6> REAPER_SETTINGS_OF_THE_SHEET{
    160, 16, 900, 400, 140, 600};

/** @brief The top speed of the Reaper, 300 units per second: 5 units in a
 * tick of a sixtieth of a second. */
constexpr float REAPER_TOP_SPEED = 300.0F;
constexpr float REAPER_UNITS_PER_TICK = 5.0F;

/** @brief A ship, the point 160 units to its right where the Reaper strikes
 * from, and a Reaper 240 units further right on the same row. */
constexpr Position HUNTED_SHIP{.x = 500.0F, .y = 300.0F};
constexpr Position STRIKING_POINT{.x = 660.0F, .y = 300.0F};
constexpr Position FAR_RIGHT_OF_THE_POINT{.x = 900.0F, .y = 300.0F};

/** @brief The horizontal share that would take it from 900 to 660 in one tick
 * of 5 units: (660 - 900) / 5. */
constexpr float SHARE_TO_THE_POINT = -48.0F;

/** @brief How long a Reaper has been idle: long enough to swing (2 s, past
 * its 900 ms rest), or not (500 ms). */
constexpr engine::Duration RESTED = std::chrono::seconds(2);
constexpr engine::Duration NOT_RESTED = std::chrono::milliseconds(500);

/** @brief The phases of the swing, and the time its strike lasts. */
constexpr ActionKey SCYTHE_SIGNAL{.attack = 0, .phase = EnemyPhase::Signal};
constexpr ActionKey SCYTHE_STRIKE{.attack = 0, .phase = EnemyPhase::Strike};
constexpr ActionKey SCYTHE_RECOVER{.attack = 0, .phase = EnemyPhase::Recover};
constexpr engine::Duration SCYTHE_STRIKE_TIME = std::chrono::milliseconds(140);

}  // namespace rtype::game::testing

#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/** @brief The settings of the shipped Bydomancer: it stops at 1600, leaps
 * 400 units, bobs 4 units every 1000 ms, rests 1500 ms, raises its staff
 * 300 ms, casts 120 ms, rests its staff 300 ms, casts its nova at a ship
 * within 192 units, for 240 ms; and its top speed. */
constexpr std::array<std::int64_t, 10> BYDOMANCER_SETTINGS_OF_THE_SHEET{
    1600, 400, 4, 1000, 1500, 300, 120, 300, 192, 240};
constexpr float BYDOMANCER_TOP_SPEED = 240.0F;

/** @brief A Bydomancer still coming in, one at its column on a row of the
 * upper half, and that column 400 units lower, where it leaps once wounded. */
constexpr Position COMING_IN{.x = 1900.0F, .y = 300.0F};
constexpr Position AT_ITS_COLUMN{.x = 1600.0F, .y = 300.0F};
constexpr Position LEAPT{.x = 1600.0F, .y = 700.0F};
constexpr float ROW_AFTER_THE_LEAP = 700.0F;

/** @brief A ship 100 units from it, within its nova reach, and one far
 * away. */
constexpr Position CLOSE_SHIP{.x = 1500.0F, .y = 300.0F};
constexpr Position FAR_SHIP{.x = 200.0F, .y = 300.0F};

/** @brief How long it has been idle: long enough for its orb. */
constexpr engine::Duration BYDOMANCER_RESTED = std::chrono::seconds(2);

/** @brief The phases of its orb and its nova. */
constexpr ActionKey ORB_SIGNAL{.attack = 0, .phase = EnemyPhase::Signal};
constexpr ActionKey ORB_STRIKE{.attack = 0, .phase = EnemyPhase::Strike};
constexpr ActionKey NOVA{.attack = 1, .phase = EnemyPhase::Strike};

}  // namespace rtype::game::testing

#pragma once

#include <cstddef>
#include <cstdint>
#include "Health.hpp"
#include "Position.hpp"

namespace rtype::game::testing {

/** @brief Where the center of a Bydo of the lowest row reaches at the bottom
 * of its weave: half a Bydo (64 units) above the status band, which starts at
 * 984 of the 1080 units of the playfield. */
constexpr float LOWEST_WAVE_CENTER_Y = 920.0F;

/** @brief Where a body that went straight from x 800 at 240 units per second
 * is after one second. */
constexpr float DRIFTER_X_AFTER_A_SECOND = 560.0F;

/** @brief Where a body that went straight from x 800 at 240 units per second,
 * heading right, is after one second. */
constexpr float RIGHTWARD_X_AFTER_A_SECOND = 1040.0F;

constexpr float HEIGHT_TOLERANCE_UNITS = 1e-3F;

/** @brief The simulation ticks of a quarter period of half a second. */
constexpr std::size_t TICKS_IN_QUARTER_PERIOD = 30;

/** @brief Two ships around an enemy that stands at the origin: one 100 units
 * to its right, one 500 units below it, farther. */
constexpr Position NEAR_SHIP{.x = 100.0F, .y = 0.0F};
constexpr Position FAR_SHIP{.x = 0.0F, .y = 500.0F};

/** @brief The network id of the enemy the view tests pilot. */
constexpr std::uint32_t WATCHING_ENEMY_NETWORK_ID = 21;

/** @brief An enemy that appeared with three points and has two left, and
 * one that still has its three. */
constexpr Health HIT_ONCE{.points = 2, .full = 3};
constexpr Health UNTOUCHED{.points = 3, .full = 3};

/** @brief Where a behaviour of the tests makes its body jump. */
constexpr Position JUMP_TARGET{.x = 640.0F, .y = 700.0F};

}  // namespace rtype::game::testing

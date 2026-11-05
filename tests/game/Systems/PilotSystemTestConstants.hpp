#pragma once

#include <cstddef>

namespace rtype::game::testing {

/** @brief Where the center of a Bydo of the lowest row reaches at the bottom
 * of its weave: half a Bydo (64 units) above the status band, which starts at
 * 984 of the 1080 units of the playfield. */
constexpr float LOWEST_WAVE_CENTER_Y = 920.0F;

/** @brief Where a body that went straight from x 800 at 240 units per second
 * is after one second. */
constexpr float DRIFTER_X_AFTER_A_SECOND = 560.0F;

constexpr float HEIGHT_TOLERANCE_UNITS = 1e-3F;

/** @brief The simulation ticks of a quarter period of half a second. */
constexpr std::size_t TICKS_IN_QUARTER_PERIOD = 30;

}  // namespace rtype::game::testing

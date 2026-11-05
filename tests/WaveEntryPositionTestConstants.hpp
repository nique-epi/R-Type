#pragma once

namespace rtype::game::testing {

/** @brief Draws of the random unit: its middle, just under 1, 1 itself and a
 * value below 0; the last two are outside the range a draw may take. */
constexpr float MIDDLE_DRAW = 0.5F;
constexpr float JUST_UNDER_ONE = 0.999999F;
constexpr float ONE = 1.0F;
constexpr float BELOW_ZERO = -0.1F;

/** @brief The lowest row a Bydo may appear on: half a Bydo (64 units) and the
 * room of a wave (48 units) above the status band, which starts at 984 of the
 * 1080 units of the playfield. */
constexpr float LOWEST_ROW_ABOVE_THE_STATUS_BAND = 872.0F;

/** @brief The highest row: half a Bydo and the room of a wave under the top
 * edge. */
constexpr float HIGHEST_ROW_UNDER_THE_TOP_EDGE = 112.0F;

/** @brief Tolerance for a row computed from a draw just under 1. */
constexpr float ROW_TOLERANCE = 0.01F;

}  // namespace rtype::game::testing

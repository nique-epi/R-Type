#pragma once

#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/** @brief The height of the frame of the ship, 16 rows. */
constexpr int SHIP_FRAME_ROWS = 16;

/** @brief Shares of a roll: its start (cos 1, every row on its back), a
 * quarter turn (cos 0, edge on: the 2 rows the ship keeps at least) and half
 * a turn (cos -1, every row on its belly). */
constexpr float START_OF_A_ROLL = 0.0F;
constexpr float QUARTER_TURN = 0.25F;
constexpr float HALF_TURN = 0.5F;
constexpr int THINNEST_ROWS_SEEN = 2;

/** @brief The rows of the upright frame the two rows of a ship seen edge on
 * show: the nearest to the middle of each half, (2 x 0 + 1) x 16 / 4 = 4 and
 * (2 x 1 + 1) x 16 / 4 = 12, and on the belly the rows that mirror them,
 * 15 - 4 = 11 and 15 - 12 = 3. */
constexpr int FIRST_ROW_EDGE_ON = 4;
constexpr int SECOND_ROW_EDGE_ON = 12;
constexpr int FIRST_ROW_EDGE_ON_BELLY = 11;
constexpr int SECOND_ROW_EDGE_ON_BELLY = 3;

/** @brief The last row of the frame. */
constexpr int LAST_ROW_OF_THE_FRAME = 15;

/** @brief Times into a roll of ROLL_DURATION (400 ms): halfway, its end, and
 * the share each reads. */
constexpr engine::Duration HALFWAY_THROUGH_A_ROLL =
    std::chrono::milliseconds(200);
constexpr engine::Duration END_OF_A_ROLL = std::chrono::milliseconds(400);
constexpr engine::Duration PAST_THE_END_OF_A_ROLL =
    std::chrono::milliseconds(450);
constexpr float HALF_OF_THE_ROLL = 0.5F;
constexpr float WHOLE_ROLL = 1.0F;

}  // namespace rtype::game::testing

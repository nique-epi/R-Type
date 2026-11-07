#pragma once

#include <cstddef>
#include <cstdint>
#include "Position.hpp"

namespace rtype::game::testing {

/**
 * @brief The burst of the Bydo Tyrant at rest at (1616, 492): five bodies
 * from its mouth, (-124, 44) from its center, so from (1492, 536), spread
 * over 48 degrees and appearing 8 units from there. Their headings are -24,
 * -12, 0, 12 and 24 degrees, a positive one turning down; the positions are
 * 1492 - 8 cos(heading) and 536 + 8 sin(heading).
 */
constexpr Position TYRANT_AT_REST_CENTER{.x = 1616.0F, .y = 492.0F};
constexpr float BURST_OFFSET_X = -124.0F;
constexpr float BURST_OFFSET_Y = 44.0F;
constexpr std::uint8_t BURST_COUNT = 5;
constexpr float BURST_SPREAD_DEGREES = 48.0F;
constexpr float BURST_DISTANCE = 8.0F;
constexpr Position BURST_ORIGIN{.x = 1492.0F, .y = 536.0F};
constexpr std::size_t SECOND_BURST_BODY = 1;
constexpr Position SECOND_BURST_BODY_POSITION{.x = 1484.1748F, .y = 534.3367F};
constexpr std::size_t MIDDLE_BURST_BODY = 2;
constexpr Position MIDDLE_BURST_BODY_POSITION{.x = 1484.0F, .y = 536.0F};
constexpr std::size_t LAST_BURST_BODY = 4;
constexpr Position LAST_BURST_BODY_POSITION{.x = 1484.6916F, .y = 539.2539F};
constexpr float PLACEMENT_TOLERANCE_UNITS = 1e-3F;

/** @brief A column of three bodies 100 units left of a center at
 * (1000, 500), 50 units apart going down: the third one at (900, 600). */
constexpr Position COLUMN_CENTER{.x = 1000.0F, .y = 500.0F};
constexpr float COLUMN_OFFSET_X = -100.0F;
constexpr std::uint8_t COLUMN_COUNT = 3;
constexpr float COLUMN_STEP_Y = 50.0F;
constexpr std::size_t THIRD_COLUMN_BODY = 2;
constexpr Position THIRD_COLUMN_BODY_POSITION{.x = 900.0F, .y = 600.0F};

/** @brief A wall of nine bodies whose second and third are its gap. */
constexpr std::uint8_t WALL_COUNT = 9;
constexpr std::uint8_t WALL_GAP_FROM = 1;
constexpr std::uint8_t WALL_GAP_COUNT = 2;
constexpr std::size_t FIRST_WALL_BODY = 0;
constexpr std::size_t SECOND_WALL_BODY = 1;
constexpr std::size_t THIRD_WALL_BODY = 2;
constexpr std::size_t FOURTH_WALL_BODY = 3;

/** @brief A burst of one body: it heads left, 8 units from the mouth. */
constexpr std::uint8_t LONE_BODY_COUNT = 1;
constexpr Position LONE_BODY_POSITION{.x = 1484.0F, .y = 536.0F};

}  // namespace rtype::game::testing

#pragma once

#include <cstdint>
#include "Position.hpp"
#include "PositionHistoryConstants.hpp"
#include "PositionSample.hpp"

namespace rtype::client::testing {

constexpr PositionSample FIRST_SAMPLE{.tick = 6,
                                      .position = {.x = 100.0F, .y = 0.0F}};
constexpr PositionSample SECOND_SAMPLE{.tick = 9,
                                       .position = {.x = 160.0F, .y = 30.0F}};

constexpr double BEFORE_THE_FIRST_SAMPLE_TICK = 2.0;

constexpr double HALFWAY_TICK = 7.5;
constexpr game::Position HALFWAY_POSITION{.x = 130.0F, .y = 15.0F};

/** @brief One and a half ticks after SECOND_SAMPLE: the last two samples move
 * 20 along x and 10 along y per tick, so the entity is 30 and 15 further. */
constexpr double ONE_AND_A_HALF_TICKS_AHEAD_TICK = 10.5;
constexpr game::Position ONE_AND_A_HALF_TICKS_AHEAD_POSITION{.x = 190.0F,
                                                             .y = 45.0F};

/** @brief 100 ticks after SECOND_SAMPLE, far beyond MAXIMUM_EXTRAPOLATION: the
 * entity stops 6 ticks past it, 120 along x and 60 along y. */
constexpr double FAR_AHEAD_TICK = 109.0;
constexpr game::Position AT_THE_EXTRAPOLATION_LIMIT{.x = 280.0F, .y = 90.0F};

/** @brief What a test reads when the history answers nothing: a position no
 * test expects. */
constexpr game::Position NO_POSITION{.x = -1.0e9F, .y = -1.0e9F};

constexpr std::uint32_t LATE_TICK = 7;
constexpr game::Position LATE_POSITION{.x = 999.0F, .y = 999.0F};

/** @brief Samples 3 ticks apart over 36 ticks, as 20 states a second give,
 * 10 units further each tick, and a tick 12 ticks behind the newest of them,
 * where the clock shows a connection whose worst lateness runs 6 ticks above
 * its usual one: the entity lies between two samples, at 245 units. */
constexpr std::uint32_t TICKS_BETWEEN_SAMPLES = 3;
constexpr std::uint32_t NEWEST_SAMPLE_TICK = 36;
constexpr double TWELVE_TICKS_BEHIND_THE_NEWEST = 24.5;
constexpr float BETWEEN_TWO_OLD_SAMPLES_X = 245.0F;
constexpr float UNITS_PER_SAMPLE_TICK = 10.0F;

/** @brief Three samples: ticks 1 and 2, then one HISTORY_SPAN_TICKS past tick
 * 3. Tick 2 is then the newest sample at or before the span, so tick 1 is
 * forgotten. */
constexpr std::uint32_t FIRST_OF_THREE_TICK = 1;
constexpr std::uint32_t OLDEST_KEPT_TICK = 2;
constexpr std::uint32_t PAST_THE_SPAN_TICK = 3 + HISTORY_SPAN_TICKS;

}  // namespace rtype::client::testing

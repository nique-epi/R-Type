#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "FinalPilot.hpp"
#include "FlightLogSchedule.hpp"
#include "TextMetrics.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns The moments of a flight log with a row per pilot and the lines
 * of @p taunt typed on its cover, at the text size of @p metrics: above the
 * size the fonts are drawn at, the rows start once the mission log of the
 * wide page was counted and held, after TEAM_DELAY, TEAM_COUNT_TIME and
 * MISSION_LOG_HOLD. */
[[nodiscard]] FlightLogSchedule flightLogScheduleOf(
    std::span<const FinalPilot> pilots, std::span<const std::string> taunt,
    TextMetrics metrics = {});

/** @returns When the row @p row (0 for the best) starts to be written. */
[[nodiscard]] engine::Duration rowStartOf(const FlightLogSchedule& schedule,
                                          std::size_t row);

/** @returns How many letters the lines of a taunt hold together. */
[[nodiscard]] std::size_t letterCountOf(std::span<const std::string> lines);

/** @returns The sounds whose moment lies after @p from and up to @p until:
 * the landing, the stamp, the start of each row, in that order. */
[[nodiscard]] std::vector<std::string_view> cuesBetween(
    const FlightLogSchedule& schedule, engine::Duration from,
    engine::Duration until);

}  // namespace rtype::client

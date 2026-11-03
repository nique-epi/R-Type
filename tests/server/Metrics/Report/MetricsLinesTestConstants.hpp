#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

namespace rtype::server::testing {

/** @brief The figures of the report the tests write out. */
constexpr std::size_t TICKS_OF_THE_REPORT = 600;
constexpr std::chrono::microseconds AVERAGE_TICK{1500};
constexpr std::chrono::microseconds MEDIAN_TICK{1250};
constexpr std::chrono::microseconds NINETY_FIFTH_PERCENTILE_TICK{2500};
constexpr std::chrono::milliseconds WORST_TICK{4};
constexpr std::size_t GAMES_HOSTED = 8;
constexpr std::size_t GAMES_IN_PROGRESS = 2;
constexpr std::size_t PLAYERS_CONNECTED = 5;
constexpr std::size_t ENTITIES_ALIVE = 143;

/** @brief Two systems of that report: over 600 ticks, 0.3 ms and 0.6 ms per
 * tick. */
constexpr std::string_view FASTER_SYSTEM_NAME = "MovementSystem";
constexpr std::string_view SLOWER_SYSTEM_NAME = "CollisionSystem";
constexpr std::chrono::milliseconds FASTER_SYSTEM_TOTAL{180};
constexpr std::chrono::milliseconds SLOWER_SYSTEM_TOTAL{360};

/** @brief What the journal must read for that report. The budget is 1 s / 60.
 */
constexpr std::string_view EXPECTED_TICKS_LINE =
    "600 ticks lasted 1.5 ms on average, 1.25 ms at the median, 2.5 ms at the "
    "95th percentile and 4 ms at most, for a budget of 16.6667 ms; games in "
    "progress: 2 of 8; players connected: 5; entities: 143";
constexpr std::string_view EXPECTED_SYSTEMS_LINE =
    "average time per tick: MovementSystem 0.3 ms, CollisionSystem 0.6 ms";

/** @brief What the journal must read for a system of a report that covers no
 * tick. */
constexpr std::string_view EXPECTED_LINE_WITHOUT_TICKS =
    "average time per tick: MovementSystem 0 ms";

}  // namespace rtype::server::testing

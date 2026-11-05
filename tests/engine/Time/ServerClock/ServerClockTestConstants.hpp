#pragma once

#include <chrono>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::engine::testing {

using std::chrono::milliseconds;
using std::chrono::seconds;

/** @brief The two ways of a path, taken to last the same. */
constexpr Duration::rep WAYS_OF_A_PATH = 2;

/** @brief The tick of the first heartbeat of the tests, 10 s after tick 0. */
constexpr std::uint32_t TEN_SECONDS_IN = 600;
/** @brief 30 ticks, half a second, later. */
constexpr std::uint32_t HALF_A_SECOND_LATER = 630;
/** @brief 60 ticks, one second, later. */
constexpr std::uint32_t ONE_SECOND_LATER = 660;

/** @brief When the server sent tick 600, in nanoseconds on the clock of the
 * client, when tick 0 left at instant 0: 600 ticks of 16 666 666 ns. */
constexpr Duration TICK_600_SENT_AT{9'999'999'600};
/** @brief When the server sent tick 630, with tick 0 at instant 0. */
constexpr Duration TICK_630_SENT_AT{10'499'999'580};
/** @brief When the server sent tick 660, with tick 0 at instant 0. */
constexpr Duration TICK_660_SENT_AT{10'999'999'560};

constexpr Duration SHORT_ROUND_TRIP = milliseconds(60);
constexpr Duration USUAL_ROUND_TRIP = milliseconds(100);
constexpr Duration LONG_ROUND_TRIP = milliseconds(500);
constexpr Duration BEST_ROUND_TRIP = milliseconds(20);

/** @brief The instant of tick 0 of a new game, far from the first one. */
constexpr Duration SECOND_GAME_TICK_ZERO = seconds(100);
/** @brief The tick the second game is at when its first heartbeat leaves. */
constexpr std::uint32_t SECOND_GAME_TICK = 30;
/** @brief Where tick 30 of the second game stands: 30 ticks after its tick 0.
 */
constexpr Duration SECOND_GAME_TICK_30_SENT_AT{100'499'999'980};

/** @brief Two milliseconds of drift, well inside the gap the clock follows. */
constexpr Duration TWO_MILLISECONDS = milliseconds(2);
/** @brief An eighth of two milliseconds. */
constexpr Duration AN_EIGHTH_OF_TWO_MILLISECONDS{250'000};
/** @brief A sample that waited in a queue is this late on the estimate. */
constexpr Duration TWO_HUNDRED_MILLISECONDS = milliseconds(200);
/** @brief Half a second, far more than the gap the clock follows. */
constexpr Duration HALF_A_SECOND = milliseconds(500);

constexpr Duration FOUR_MILLISECONDS = milliseconds(4);
/** @brief An eighth of four milliseconds. */
constexpr Duration AN_EIGHTH_OF_FOUR_MILLISECONDS{500'000};

/** @brief The way out of a path 10 ms long, and the way back, 50 ms long. */
constexpr Duration SHORT_WAY = milliseconds(10);
constexpr Duration LONG_WAY = milliseconds(50);
/** @brief Half the difference of the two ways: what an asymmetric path costs
 * the estimate. */
constexpr Duration HALF_THE_DIFFERENCE = milliseconds(20);

/** @brief The samples that follow the best one in the test of the window,
 * one fewer than fill it. */
constexpr std::uint32_t SAMPLES_BEFORE_THE_BEST_LEAVES = 7;

}  // namespace rtype::engine::testing

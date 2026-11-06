#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

/** @brief The probe module that makes each tick last a known time on the clock
 * of the metrics, and that time. */
constexpr const char* METRICS_CLOCK_PROBE_MODULE = "metrics clock probe";
constexpr std::chrono::milliseconds TICK_WORK_TIME{3};

/** @brief The ticks of a report but one, and of two reports. */
constexpr std::size_t ONE_TICK_BEFORE_THE_REPORT = 599;
constexpr std::size_t TICKS_OF_TWO_REPORTS = 1200;

/** @brief What marks a line of the metrics in the journal, and how many lines a
 * report writes: the ticks, the systems and the round trips. */
constexpr const char* METRICS_LINE_MARK = "[Metrics]";
constexpr std::size_t LINES_OF_ONE_REPORT = 3;
constexpr std::size_t LINES_OF_TWO_REPORTS = 6;

/** @brief The round trip a test gives a player, and the number the metrics
 * give its game, the one game of the process, numbered from 1. */
constexpr std::chrono::milliseconds MEASURED_ROUND_TRIP{42};
constexpr std::size_t NUMBER_OF_THE_ONLY_GAME = 1;

/** @brief The games a game process hosts. */
constexpr std::size_t GAMES_OF_THE_PROCESS = 1;

/** @brief The system of a known time the tests add to a game, how long one run
 * of it takes, and how long a run of two such systems takes. */
constexpr std::string_view TIMED_SYSTEM_NAME = "timed";
constexpr std::chrono::milliseconds TIMED_SYSTEM_TIME{3};
constexpr std::chrono::milliseconds TIMED_SYSTEM_TIME_OF_TWO_SYSTEMS{6};

/** @brief Each tick moves the clock by TICK_WORK_TIME (3 ms), so the traffic
 * line, due at 10 s of the clock, comes after 3334 ticks (10.002 s), not 3333
 * (9.999 s); the rows, due at 1 s, after 334 ticks (1.002 s). */
constexpr std::size_t TICKS_JUST_BEFORE_THE_TRAFFIC_LINE = 3333;
constexpr std::size_t TICKS_TO_THE_TRAFFIC_ROWS = 334;

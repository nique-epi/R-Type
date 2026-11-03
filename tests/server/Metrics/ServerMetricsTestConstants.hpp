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
 * report writes. */
constexpr const char* METRICS_LINE_MARK = "[Metrics]";
constexpr std::size_t LINES_OF_ONE_REPORT = 2;
constexpr std::size_t LINES_OF_TWO_REPORTS = 4;

/** @brief The system of a known time the tests add to a game, how long one run
 * of it takes, and how long a run in each of two games takes. */
constexpr std::string_view TIMED_SYSTEM_NAME = "timed";
constexpr std::chrono::milliseconds TIMED_SYSTEM_TIME{3};
constexpr std::chrono::milliseconds TIMED_SYSTEM_TIME_IN_TWO_GAMES{6};

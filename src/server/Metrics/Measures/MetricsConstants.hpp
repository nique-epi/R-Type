#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::server {

/** @brief Logger name the metrics of the server write their log lines under. */
constexpr std::string_view METRICS_LOGGER_NAME = "Metrics";

/** @brief The percentiles a report gives of the durations of the ticks, and the
 * whole they are counted out of. */
constexpr std::size_t MEDIAN_PERCENT = 50;
constexpr std::size_t NINETY_FIFTH_PERCENT = 95;
constexpr std::size_t WHOLE_PERCENT = 100;

/** @brief The games a game process hosts, as its census counts them. */
constexpr std::size_t GAMES_OF_A_GAME_PROCESS = 1;

/** @brief The number the round trips give the game of a game process: the
 * lines number games from 1. */
constexpr std::size_t NUMBER_OF_THE_GAME = 1;

}  // namespace rtype::server

#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief How long the bets of a round stay open while a player sits at the
 * table, unless every seated player asks to spin sooner.
 */
constexpr engine::Duration ROULETTE_BETTING_TIME = std::chrono::seconds(20);

/** @brief How long the wheel turns once the number is drawn. */
constexpr engine::Duration ROULETTE_SPINNING_TIME = std::chrono::seconds(6);

/** @brief How long the result of a round shows before the next round. */
constexpr engine::Duration ROULETTE_RESULT_TIME = std::chrono::seconds(5);

/** @brief The three times above, in whole simulation ticks. */
constexpr std::size_t ROULETTE_BETTING_TICKS =
    engine::wholeTicksIn(ROULETTE_BETTING_TIME);
constexpr std::size_t ROULETTE_SPINNING_TICKS =
    engine::wholeTicksIn(ROULETTE_SPINNING_TIME);
constexpr std::size_t ROULETTE_RESULT_TICKS =
    engine::wholeTicksIn(ROULETTE_RESULT_TIME);

static_assert(ROULETTE_BETTING_TICKS <=
                  std::numeric_limits<std::uint16_t>::max(),
              "the ticks left of a round are sent on two bytes");

/**
 * @brief The time between two RouletteRound a player gets while nothing but
 * the clock changes: the same as a LobbyState.
 */
constexpr engine::Duration ROULETTE_ROUND_INTERVAL =
    std::chrono::milliseconds(500);
constexpr std::size_t TICKS_PER_ROULETTE_ROUND =
    engine::wholeTicksIn(ROULETTE_ROUND_INTERVAL);

/** @brief The name of the logger the casino writes its draws under. */
constexpr std::string_view CASINO_LOGGER_NAME = "Casino";

}  // namespace rtype::server

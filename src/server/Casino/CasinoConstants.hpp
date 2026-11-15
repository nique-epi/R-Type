#pragma once

#include <array>
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
 * @brief The most chips one player may lay on the cloth in one round: a
 * RouletteBets whose bets add up to more is refused whole.
 */
constexpr std::uint32_t MAXIMUM_ROUND_STAKE = 5000;

/**
 * @brief What a winning bet returns per chip staked, its stake included: 35 to
 * 1 for a straight, 17 for a split, 11 for a street, 8 for a corner, 5 for a
 * six line, 2 for a dozen or a column, 1 for an even chance.
 */
constexpr std::uint32_t STRAIGHT_RETURN = 36;
constexpr std::uint32_t SPLIT_RETURN = 18;
constexpr std::uint32_t STREET_RETURN = 12;
constexpr std::uint32_t CORNER_RETURN = 9;
constexpr std::uint32_t SIX_LINE_RETURN = 6;
constexpr std::uint32_t DOZEN_RETURN = 3;
constexpr std::uint32_t EVEN_CHANCE_RETURN = 2;

static_assert(static_cast<std::uint64_t>(MAXIMUM_ROUND_STAKE) *
                      STRAIGHT_RETURN <=
                  std::numeric_limits<std::uint32_t>::max(),
              "the payout of a seat is sent on four bytes");

/** @brief The lowest number of the cloth after the zero. */
constexpr std::uint8_t LOWEST_CLOTH_NUMBER = 1;

/** @brief The red numbers of the wheel; every other number but the zero is
 * black. */
constexpr auto RED_NUMBERS = std::to_array<std::uint8_t>(
    {1, 3, 5, 7, 9, 12, 14, 16, 18, 19, 21, 23, 25, 27, 30, 32, 34, 36});

/** @brief The numbers of a street, the row of three across the cloth, and of a
 * dozen; the last number of the low half. */
constexpr std::uint8_t NUMBERS_PER_STREET = 3;
constexpr std::uint8_t NUMBERS_PER_DOZEN = 12;
constexpr std::uint8_t LAST_LOW_NUMBER = 18;

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

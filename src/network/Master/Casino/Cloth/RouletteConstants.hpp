#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace rtype::network {

/**
 * @brief The highest number of the roulette wheel: a single zero and 1 to 36,
 * 37 pockets.
 */
constexpr std::uint8_t HIGHEST_ROULETTE_NUMBER = 36;

/**
 * @brief The bets a player may hold on the cloth in one round, at most.
 */
constexpr std::size_t MAXIMUM_ROULETTE_BETS = 32;

/**
 * @brief The most chips one player may lay on the cloth in one round: bets
 * that add up to more are refused whole.
 */
constexpr std::uint32_t MAXIMUM_ROUND_STAKE = 5000;

/** @brief The lowest number of the cloth after the zero. */
constexpr std::uint8_t LOWEST_CLOTH_NUMBER = 1;

/** @brief The red numbers of the wheel; every other number but the zero is
 * black. */
constexpr auto RED_ROULETTE_NUMBERS = std::to_array<std::uint8_t>(
    {1, 3, 5, 7, 9, 12, 14, 16, 18, 19, 21, 23, 25, 27, 30, 32, 34, 36});

/** @brief The numbers of a street, the row of three across the cloth, and of a
 * dozen; the last number of the low half. */
constexpr std::uint8_t NUMBERS_PER_STREET = 3;
constexpr std::uint8_t NUMBERS_PER_DOZEN = 12;
constexpr std::uint8_t LAST_LOW_NUMBER = 18;

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
              "the payout of a round fits the four bytes of a CasinoPlayer");

}  // namespace rtype::network

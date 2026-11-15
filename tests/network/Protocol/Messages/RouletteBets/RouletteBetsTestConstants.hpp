#pragma once

#include <cstdint>

namespace rtype::network::testing {

/** @brief A round number of two distinct bytes, 0x0102, as the protocol page
 * writes one. */
constexpr std::uint16_t SOME_ROULETTE_ROUND = 0x0102;

/** @brief A straight bet of 500 chips (0x01F4) on 17. */
constexpr std::uint8_t SEVENTEEN = 17;
constexpr std::uint32_t FIVE_HUNDRED_CHIPS = 500;

/** @brief One past the highest number of the wheel, and one past the last kind
 * of bet. */
constexpr std::uint8_t NUMBER_PAST_THE_WHEEL = 37;
constexpr std::uint8_t KIND_PAST_THE_LAST = 14;

/** @brief A bet of a single chip, the smallest a player may lay. */
constexpr std::uint32_t ONE_CHIP = 1;

}  // namespace rtype::network::testing

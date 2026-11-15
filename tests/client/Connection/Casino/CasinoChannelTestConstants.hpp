#pragma once

#include <cstdint>

namespace rtype::client::testing {

/** @brief Two rounds of the table, the second right after the first. */
constexpr std::uint16_t SOME_ROUND = 5;
constexpr std::uint16_t ROUND_BEFORE = 4;

/** @brief A stake of 100 chips and what it returns straight on the number
 * drawn, 3600. */
constexpr std::uint32_t STAKE_OF_A_HUNDRED = 100;
constexpr std::uint32_t PAYOUT_OF_A_HUNDRED = 3600;

/** @brief The number a straight bet of the tests lies on. */
constexpr std::uint8_t BET_NUMBER = 17;

}  // namespace rtype::client::testing

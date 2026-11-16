#pragma once

#include <cstdint>
#include <limits>
#include "ChipMoves.hpp"
#include "KopecksConstants.hpp"

namespace rtype::client::testing {

/** @brief A player with a thousand chips, a round that took 100 of them and
 * paid 350 back, and what is left after it: 1000 - 100 + 350. */
constexpr std::uint32_t A_THOUSAND_CHIPS = 1000;
constexpr ChipMoves A_ROUND_WON{.staked = 100, .won = 350};
constexpr std::uint32_t CHIPS_AFTER_THE_ROUND_WON = 1250;

/** @brief A player with fewer chips than a stake the server settled. */
constexpr std::uint32_t FIFTY_CHIPS = 50;
constexpr ChipMoves A_STAKE_OF_A_HUNDRED_LOST{.staked = 100, .won = 0};

/** @brief A player ten chips under the largest balance, and a round that pays
 * a hundred, ninety more than room is left for. */
constexpr std::uint32_t TEN_CHIPS_UNDER_THE_LARGEST =
    rtype::network::MAXIMUM_CHIPS - 10;
constexpr ChipMoves A_HUNDRED_WON{.staked = 0, .won = 100};

/** @brief A payout as large as the count of the server can be. */
constexpr ChipMoves THE_LARGEST_PAYOUT{
    .staked = 0, .won = std::numeric_limits<std::uint64_t>::max()};

}  // namespace rtype::client::testing

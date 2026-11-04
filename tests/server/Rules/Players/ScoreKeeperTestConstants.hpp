#pragma once

#include <cstdint>
#include <limits>
#include "ProtocolConstants.hpp"

namespace rtype::server::testing {

/** @brief The numbers of the two players of the score tests. */
constexpr std::uint8_t FIRST_NUMBER = network::FIRST_PLAYER_NUMBER;
constexpr std::uint8_t SECOND_NUMBER = network::FIRST_PLAYER_NUMBER + 1;

/** @brief The points the kill of a Drifter and of a Weaver is worth in the
 * score tests. */
constexpr std::uint32_t DRIFTER_WORTH = 100;
constexpr std::uint32_t WEAVER_WORTH = 200;

/** @brief The largest score a player can hold, and the points below it a
 * player starts from in the test of the largest score. */
constexpr std::uint32_t LARGEST_SCORE =
    std::numeric_limits<std::uint32_t>::max();
constexpr std::uint32_t TEN_POINTS = 10;

}  // namespace rtype::server::testing

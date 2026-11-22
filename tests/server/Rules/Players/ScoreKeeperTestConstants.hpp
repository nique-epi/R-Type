#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include "GameRuleConstants.hpp"
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

/** @brief A boss worth 3 000 points, three damage of the first pilot and one
 * of the second: 2 250 and 750. */
constexpr std::uint32_t BOSS_WORTH = 3'000;
constexpr std::array<std::uint32_t, identifiers::MAXIMUM_PLAYERS>
    THREE_THEN_ONE{3, 1, 0, 0};
constexpr std::uint32_t THREE_QUARTERS_OF_THE_BOSS = 2'250;
constexpr std::uint32_t A_QUARTER_OF_THE_BOSS = 750;

/** @brief 1 000 points, one damage of the first pilot and two of the second:
 * 333 and 666 by the damage, and the 1 left over to the first, who destroyed
 * it. */
constexpr std::uint32_t THOUSAND_POINTS = 1'000;
constexpr std::array<std::uint32_t, identifiers::MAXIMUM_PLAYERS> ONE_THEN_TWO{
    1, 2, 0, 0};
constexpr std::uint32_t A_THIRD_AND_THE_REST = 334;
constexpr std::uint32_t TWO_THIRDS = 666;

/** @brief One damage of each of three pilots, the third of whom is not in
 * the roster: a third of the boss each, 1 000. */
constexpr std::array<std::uint32_t, identifiers::MAXIMUM_PLAYERS>
    ONE_EACH_OF_THREE{1, 1, 1, 0};
constexpr std::uint32_t A_THIRD_OF_THE_BOSS = 1'000;

/** @brief The bounty of the Ash Mother, the largest bounty a player holds,
 * and the kopecks below it a player starts from in the test of the largest
 * bounty. */
constexpr std::uint32_t ASH_MOTHER_BOUNTY = 550;
constexpr std::uint32_t LARGEST_BOUNTY =
    std::numeric_limits<std::uint32_t>::max();
constexpr std::uint32_t TEN_KOPECKS = 10;

}  // namespace rtype::server::testing

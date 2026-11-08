#pragma once

#include <chrono>
#include <cstdint>
#include "BossStatus.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief The boss of the tracker tests: entity 31, 240 points, then 230,
 * 220, 120 (half of them) and 100 left. */
constexpr std::uint32_t TRACKED_BOSS_ID = 31;
constexpr std::uint16_t TRACKED_FULL_HEALTH = 240;
constexpr std::uint16_t AFTER_ONE_HIT = 230;
constexpr std::uint16_t AFTER_TWO_HITS = 220;
constexpr std::uint16_t AT_HALF_HEALTH = 120;
constexpr std::uint16_t BELOW_HALF_HEALTH = 100;

/** @brief The times the statuses arrive at, on the clock of the tracker. */
constexpr engine::Duration AT_THE_START = engine::Duration::zero();
constexpr engine::Duration A_TENTH_OF_A_SECOND_IN =
    std::chrono::milliseconds(100);
constexpr engine::Duration FOUR_TENTHS_IN = std::chrono::milliseconds(400);
constexpr engine::Duration SIX_TENTHS_IN = std::chrono::milliseconds(600);
constexpr engine::Duration A_SECOND_IN = std::chrono::seconds(1);
constexpr engine::Duration A_SECOND_AND_A_HALF_IN =
    std::chrono::milliseconds(1500);
constexpr engine::Duration HALF_A_SECOND = std::chrono::milliseconds(500);
constexpr engine::Duration TWO_SECONDS_IN = std::chrono::seconds(2);

/** @brief The status of a boss arriving. */
constexpr network::BossStatus ARRIVING{.stage = network::BossStage::Arriving};

}  // namespace rtype::client::testing

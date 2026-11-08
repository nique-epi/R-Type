#pragma once

#include <cstdint>

namespace rtype::network::testing {

/** @brief The boss of the status tests: entity 9, with 600 points, half of
 * them left, then one more than full. */
constexpr std::uint32_t BOSS_ENTITY_ID = 9;
constexpr std::uint16_t BOSS_FULL_HEALTH = 600;
constexpr std::uint16_t BOSS_HALF_HEALTH = 300;
constexpr std::uint16_t BOSS_HEALTH_ABOVE_FULL = 601;
constexpr std::uint16_t BOSS_LAST_POINT = 1;

}  // namespace rtype::network::testing

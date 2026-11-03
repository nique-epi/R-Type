#pragma once

#include <cstdint>
#include <limits>

namespace rtype::client::testing {

/**
 * @brief The newest tick a filter has applied in the tests, and the ticks
 * around it.
 */
constexpr std::uint32_t APPLIED_TICK = 10;
constexpr std::uint32_t NEWER_TICK = 11;
constexpr std::uint32_t OLDER_TICK = 9;
constexpr std::uint32_t OLDEST_TICK = 8;

/**
 * @brief The first tick of a game.
 */
constexpr std::uint32_t FIRST_TICK = 0;

/**
 * @brief The largest tick a 32-bit counter holds.
 */
constexpr std::uint32_t LARGEST_TICK =
    std::numeric_limits<std::uint32_t>::max();

}  // namespace rtype::client::testing

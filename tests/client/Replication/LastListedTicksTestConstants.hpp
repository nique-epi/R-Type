#pragma once

#include <cstdint>

namespace rtype::client::testing {

/**
 * @brief The protocol page removes an entity no WorldState listed for 900 ms:
 * 54 server ticks at 60 per second.
 */
constexpr std::uint32_t TICKS_IN_900_MILLISECONDS = 54;

/**
 * @brief The tick of the newest state that lists an entity in the tests. It is
 * past TICKS_IN_900_MILLISECONDS, so an entity counted from the start of the
 * game would already be removed at it.
 */
constexpr std::uint32_t LISTING_TICK = 100;

/**
 * @brief The first tick at which an entity last listed at LISTING_TICK is
 * removed, and the tick just before, at which it is kept.
 */
constexpr std::uint32_t REMOVAL_TICK = LISTING_TICK + TICKS_IN_900_MILLISECONDS;
constexpr std::uint32_t LAST_KEPT_TICK = REMOVAL_TICK - 1;

}  // namespace rtype::client::testing

#pragma once

#include <cstddef>

namespace rtype::server {

/** @brief How often a wave brings a look item when a wave plan says nothing:
 * each wave from the second. */
constexpr std::size_t LOOK_ITEM_EVERY_WAVE = 1;

/** @brief The fewest waves between two look items, whatever a plan says: a
 * plan built by hand with 0 brings one each wave. */
constexpr std::size_t FEWEST_WAVES_PER_LOOK_ITEM = 1;

/** @brief How often a wave brings the crystal carrier of a plan when the plan
 * names one and says nothing of how often: every other wave, from the second.
 */
constexpr std::size_t CARRIER_EVERY_WAVE = 2;

/** @brief The fewest waves between two crystal carriers, whatever a plan
 * says: a plan built by hand with 0 brings one each wave. */
constexpr std::size_t FEWEST_WAVES_PER_CARRIER = 1;

}  // namespace rtype::server

#pragma once

#include <cstddef>

namespace rtype::server {

/** @brief How often a wave brings a look item when a wave plan says nothing:
 * each wave from the second. */
constexpr std::size_t LOOK_ITEM_EVERY_WAVE = 1;

/** @brief The fewest waves between two look items, whatever a plan says: a
 * plan built by hand with 0 brings one each wave. */
constexpr std::size_t FEWEST_WAVES_PER_LOOK_ITEM = 1;

}  // namespace rtype::server

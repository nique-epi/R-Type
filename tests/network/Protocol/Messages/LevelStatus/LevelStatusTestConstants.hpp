#pragma once

#include <limits>
#include <string_view>

namespace rtype::network::testing {

/** @brief The level of the status tests: the biome `wreck`, scrolled 240
 * logical units, at 120 units per second, its biome begun at 96 units. */
constexpr std::string_view STATUS_BIOME = "wreck";
constexpr float STATUS_SCROLL_POSITION = 240.0F;
constexpr float STATUS_SCROLL_SPEED = 120.0F;
constexpr float STATUS_BIOME_SINCE = 96.0F;

/** @brief A scroll that is paused, and values no scroll can have. */
constexpr float PAUSED_SCROLL_SPEED = 0.0F;
constexpr float NEGATIVE_SCROLL = -1.0F;
constexpr float SCROLL_THAT_IS_NO_NUMBER =
    std::numeric_limits<float>::quiet_NaN();

/** @brief A name no biome can have: it holds a capital. */
constexpr std::string_view CAPITALIZED_BIOME = "Wreck";

}  // namespace rtype::network::testing

#pragma once

#include <cstdint>
#include <limits>
#include <string_view>

namespace rtype::network::testing {

/** @brief The level of the status tests: the biome `wreck`, scrolled 240
 * logical units, at 120 units per second, its biome begun at 96 units. */
constexpr std::string_view STATUS_BIOME = "wreck";
constexpr float STATUS_SCROLL_POSITION = 240.0F;
constexpr float STATUS_SCROLL_SPEED = 120.0F;
constexpr float STATUS_BIOME_SINCE = 96.0F;

/** @brief The time left of a transition, 2500 ms, the bytes 09 C4, and the
 * longest one a status carries. */
constexpr std::uint16_t STATUS_TRANSITION_LEFT = 2500;
constexpr std::uint16_t LONGEST_TRANSITION_LEFT =
    std::numeric_limits<std::uint16_t>::max();

/** @brief The band of the status tests, the rows of the Pit: its top on row
 * 192, the bytes 00 C0, and its bottom on row 848, the bytes 03 50. */
constexpr std::uint16_t STATUS_BAND_TOP = 192;
constexpr std::uint16_t STATUS_BAND_BOTTOM = 848;

/** @brief Continues left to the team of a status: 2 of its 3, all 3, and one
 * more than a team ever gets. */
constexpr std::uint8_t STATUS_CONTINUES_LEFT = 2;
constexpr std::uint8_t ALL_THE_CONTINUES = 3;
constexpr std::uint8_t ONE_CONTINUE_TOO_MANY = 4;

/** @brief A scroll that is paused, and values no scroll can have. */
constexpr float PAUSED_SCROLL_SPEED = 0.0F;
constexpr float NEGATIVE_SCROLL = -1.0F;
constexpr float SCROLL_THAT_IS_NO_NUMBER =
    std::numeric_limits<float>::quiet_NaN();

/** @brief A name no biome can have: it holds a capital. */
constexpr std::string_view CAPITALIZED_BIOME = "Wreck";

}  // namespace rtype::network::testing

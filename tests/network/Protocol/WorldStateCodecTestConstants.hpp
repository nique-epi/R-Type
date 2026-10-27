#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::network::testing {

/** @brief A motion: the pilot holds right (bit 3) and its ship rolls (bit 4).
 */
constexpr std::uint8_t ROLLING_RIGHT_MOTION = 0x18;

/** @brief A motion with bit 6 set, past the bit of the dash. */
constexpr std::uint8_t MOTION_PAST_THE_DASH = 0x40;

/** @brief Where the motion of the first player sits in a payload: tick (4),
 * playerCount (1), then playerNumber (1), shipEntityId (4), lives (1),
 * score (4) and invulnerable (1). */
constexpr std::size_t FIRST_MOTION_OFFSET = 16;

/** @brief Bytes of the payload of the largest state one datagram holds, four
 * players and ENTITIES_FITTING_WITH_FOUR_PLAYERS entities: 6 + 4 * 12 + 124
 * * 9. The 5 bytes left before 1175 hold no whole entity. */
constexpr std::size_t LARGEST_STATE_PAYLOAD = 1170;

}  // namespace rtype::network::testing

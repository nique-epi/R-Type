#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

namespace rtype::server::testing {

constexpr std::uint32_t MONSTER_NETWORK_ID = 7;
constexpr float MONSTER_X = 100.0F;
constexpr float MONSTER_Y = 50.0F;
constexpr std::uint8_t SECOND_PLAYER_NUMBER = 2;

/**
 * @brief The largest event number: the next one wraps to 0.
 */
constexpr std::uint16_t LARGEST_EVENT_NUMBER = 65535;

/**
 * @brief Events that carry the number 1 to LARGEST_EVENT_NUMBER, then one more
 * that wraps to 0, then one more that carries 1 again.
 */
constexpr std::size_t EVENTS_PAST_THE_WRAP =
    std::size_t{LARGEST_EVENT_NUMBER} + 2;

/**
 * @brief Values the protocol refuses: a coordinate that is not a number, the
 * network id no entity has, and a player number outside 1 to 4.
 */
constexpr float NOT_A_NUMBER = std::numeric_limits<float>::quiet_NaN();
constexpr std::uint32_t UNASSIGNED_NETWORK_ID = 0;
constexpr std::uint8_t PLAYER_NUMBER_OUTSIDE_THE_GAME = 0;

}  // namespace rtype::server::testing

#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include "GameEventConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

constexpr std::uint16_t FIRST_CLIENT_PORT = 4000;
constexpr std::uint16_t SECOND_CLIENT_PORT = 4001;
constexpr std::uint32_t MONSTER_NETWORK_ID = 7;
constexpr float MONSTER_X = 100.0F;
constexpr float MONSTER_Y = 50.0F;
constexpr std::uint8_t SECOND_PLAYER_NUMBER = 2;

static_assert(REPEATED_MESSAGE_INTERVAL > std::chrono::milliseconds(1),
              "the tests stop one millisecond short of an interval");

/**
 * @brief Just short of one interval: a copy due at the interval is not sent
 * yet.
 */
constexpr engine::Duration JUST_BEFORE_ONE_INTERVAL =
    REPEATED_MESSAGE_INTERVAL - std::chrono::milliseconds(1);
constexpr engine::Duration ONE_INTERVAL = REPEATED_MESSAGE_INTERVAL;

/**
 * @brief The copies of a message no state repeats that the protocol page
 * sends: three.
 */
constexpr std::size_t PROTOCOL_REPEATED_MESSAGE_COPIES = 3;

/**
 * @brief Long enough for every copy to be sent, and for none to be left.
 */
constexpr engine::Duration ALL_COPIES_SENT =
    REPEATED_MESSAGE_INTERVAL *
    static_cast<engine::Duration::rep>(REPEATED_MESSAGE_COPIES);

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

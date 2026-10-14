#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::server::testing {

constexpr std::uint16_t FIRST_CLIENT_PORT = 4000;
constexpr std::uint16_t SECOND_CLIENT_PORT = 4001;
constexpr std::uint32_t MONSTER_NETWORK_ID = 7;
constexpr float MONSTER_X = 100.0F;
constexpr float MONSTER_Y = 50.0F;
constexpr std::uint8_t SECOND_PLAYER_NUMBER = 2;

/**
 * @brief Just short of one interval: a copy due at the interval is not sent
 * yet.
 */
constexpr engine::Duration JUST_BEFORE_ONE_INTERVAL =
    std::chrono::milliseconds(49);
constexpr engine::Duration ONE_INTERVAL = std::chrono::milliseconds(50);

/**
 * @brief Long enough for every copy to be sent, and for none to be left.
 */
constexpr engine::Duration ONE_SECOND = std::chrono::seconds(1);

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

}  // namespace rtype::server::testing

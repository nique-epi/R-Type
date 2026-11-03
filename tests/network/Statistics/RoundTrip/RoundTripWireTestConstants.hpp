#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::network::testing {

/**
 * @brief A player and its figures: a round trip of 42.1 ms and a jitter of
 * 3.2 ms, which RoundTrips carries as 42100 µs and 3200 µs.
 */
constexpr std::uint8_t WIRE_PLAYER_NUMBER = 2;
constexpr std::chrono::microseconds WIRE_ROUND_TRIP{42'100};
constexpr std::chrono::microseconds WIRE_JITTER{3'200};
constexpr std::uint32_t WIRE_ROUND_TRIP_ON_THE_WIRE = 42'100;
constexpr std::uint32_t WIRE_JITTER_ON_THE_WIRE = 3'200;

/**
 * @brief A time 1.5 µs after 2^32 µs: on the wire, the whole microseconds past
 * the wrap of the counter, 1.
 */
constexpr std::chrono::nanoseconds PAST_THE_WRAP{4'294'967'297'500};
constexpr std::uint32_t PAST_THE_WRAP_ON_THE_WIRE = 1;

}  // namespace rtype::network::testing

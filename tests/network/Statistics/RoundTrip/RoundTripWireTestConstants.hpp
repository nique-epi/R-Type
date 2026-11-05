#pragma once

#include <chrono>
#include <cstdint>
#include "RoundTripMeterTestConstants.hpp"

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

/**
 * @brief A request asked at 10 ms on the clock of the asker, held 3 ms by the
 * server, its answer back at 50 ms: a round trip of 37 ms. The server holds
 * one asked at 49 ms for 3 ms, longer than it had been on its way.
 */
constexpr std::chrono::nanoseconds ASKED_AT_TEN_MILLISECONDS{
    std::chrono::milliseconds{10}};
constexpr std::chrono::nanoseconds ASKED_AT_FORTY_NINE_MILLISECONDS{
    std::chrono::milliseconds{49}};
constexpr std::chrono::nanoseconds ANSWERED_AT_FIFTY_MILLISECONDS{
    std::chrono::milliseconds{50}};
constexpr std::chrono::microseconds THIRTY_SEVEN_MILLISECONDS{37'000};

/** @brief An answer back 21 s after its request, past the longest round trip
 * of 20 s. */
constexpr std::chrono::nanoseconds TWENTY_ONE_SECONDS_LATER =
    ASKED_AT_TEN_MILLISECONDS + std::chrono::seconds{21};

/** @brief A request asked ONE_MILLISECOND_BEFORE_THE_WRAP of the counter of
 * microseconds, its answer back 2 ms after the wrap, held 0: a round trip of
 * 3 ms. */
constexpr std::chrono::microseconds THREE_MILLISECONDS{3'000};
constexpr std::chrono::nanoseconds TWO_MILLISECONDS_AFTER_THE_WRAP =
    ONE_MILLISECOND_BEFORE_THE_WRAP + THREE_MILLISECONDS;

}  // namespace rtype::network::testing

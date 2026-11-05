#pragma once

#include <chrono>
#include <cstdint>
#include "RoundTripFigures.hpp"
#include "RoundTrips.hpp"

namespace rtype::network {

/** @returns @p time in whole microseconds, modulo 2^32, as a Heartbeat
 * carries it. */
[[nodiscard]] std::uint32_t wireMicrosecondsOf(std::chrono::nanoseconds time);

/**
 * @returns The round trip of the player numbered @p playerNumber as RoundTrips
 * carries it: @p figures in whole microseconds. Both figures of a
 * RoundTripMeter are at most LONGEST_ROUND_TRIP, which fits in 32 bits.
 */
[[nodiscard]] PlayerRoundTrip playerRoundTripOf(
    std::uint8_t playerNumber, const RoundTripFigures& figures);

/** @returns The figures RoundTrips carries for @p player. */
[[nodiscard]] RoundTripFigures figuresOf(const PlayerRoundTrip& player);

}  // namespace rtype::network

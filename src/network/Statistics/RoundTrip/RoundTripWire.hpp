#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include "Heartbeat.hpp"
#include "RoundTripFigures.hpp"
#include "RoundTrips.hpp"
#include "ServerInformation.hpp"
#include "ServerInformationRequest.hpp"

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

/**
 * @returns The round trip @p echo gives, read at @p receivedAt on the clock
 * that wrote its sentAt: the time since sentAt, less the time the peer held
 * it. Nothing when it was held longer than since it was sent, or when the
 * round trip is longer than LONGEST_ROUND_TRIP.
 */
[[nodiscard]] std::optional<std::chrono::microseconds> echoedRoundTrip(
    const HeartbeatEcho& echo, std::chrono::nanoseconds receivedAt);

/** @returns The ServerInformationRequest to send at @p now, which a
 * ServerInformation echoes. */
[[nodiscard]] ServerInformationRequest informationRequestAt(
    std::chrono::nanoseconds now);

/**
 * @returns The round trip to the server that sent @p answer, which arrived at
 * @p receivedAt on the clock of the request it echoes (see
 * informationRequestAt()), as echoedRoundTrip() reads it: the time the server
 * held the request is left out, so it is the ping of the list of servers and
 * of the probe of the master. Nothing in the cases of echoedRoundTrip().
 */
[[nodiscard]] std::optional<std::chrono::microseconds> informationRoundTripOf(
    const ServerInformation& answer, std::chrono::nanoseconds receivedAt);

}  // namespace rtype::network

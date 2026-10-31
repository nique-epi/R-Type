#pragma once

#include <chrono>
#include <cstddef>
#include <vector>
#include "DatagramInFlight.hpp"
#include "DatagramsInFlight.hpp"

/** @brief Hands @p count datagrams to @p inFlight, @p spacing apart from
 * HAND_OVER_START, the payload of each holding its number, from 0. */
void handNumbered(rtype::network::DatagramsInFlight& inFlight,
                  std::size_t count, std::chrono::milliseconds spacing);

/** @returns Every datagram in flight, in the order they arrive, however
 * late. */
[[nodiscard]] std::vector<rtype::network::DatagramInFlight> everyArrival(
    rtype::network::DatagramsInFlight& inFlight);

/** @returns The number handNumbered() wrote in @p datagram. */
[[nodiscard]] std::size_t numberOf(
    const rtype::network::DatagramInFlight& datagram);

/** @returns How long @p datagram, handed over by handNumbered() with
 * @p spacing, waited before it arrived. */
[[nodiscard]] std::chrono::steady_clock::duration waitOf(
    const rtype::network::DatagramInFlight& datagram,
    std::chrono::milliseconds spacing);

/** @returns The longest and the shortest wait of @p arrivals, handed over by
 * handNumbered() with @p spacing. */
[[nodiscard]] std::chrono::steady_clock::duration longestWait(
    const std::vector<rtype::network::DatagramInFlight>& arrivals,
    std::chrono::milliseconds spacing);
[[nodiscard]] std::chrono::steady_clock::duration shortestWait(
    const std::vector<rtype::network::DatagramInFlight>& arrivals,
    std::chrono::milliseconds spacing);

/** @returns How many of @p arrivals, handed over by handNumbered() with
 * @p spacing, waited exactly @p wait. */
[[nodiscard]] std::size_t countWaiting(
    const std::vector<rtype::network::DatagramInFlight>& arrivals,
    std::chrono::steady_clock::duration wait,
    std::chrono::milliseconds spacing);

/** @returns For each number from 0 to @p count - 1, how many times it is in
 * @p arrivals. */
[[nodiscard]] std::vector<std::size_t> arrivalsByNumber(
    const std::vector<rtype::network::DatagramInFlight>& arrivals,
    std::size_t count);

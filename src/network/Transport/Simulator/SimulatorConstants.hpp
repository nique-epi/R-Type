#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::network {

/**
 * @brief How many times a round trip crosses the simulated network: once
 * out, once back, so each crossing takes half the lag and the jitter.
 */
constexpr int CROSSINGS_PER_ROUND_TRIP = 2;

/** @brief How many times a copied datagram arrives. */
constexpr int COPIED_DATAGRAM_ARRIVALS = 2;

/**
 * @brief Most datagrams one direction of a simulated network holds at once,
 * copies included. One handed over when that many wait is lost, as in the full
 * queue of a router, so a flood in one direction holds at most this many times
 * MAX_DATAGRAM_SIZE bytes of payload, about 10 MB, and one in both directions
 * about 20 MB.
 *
 * At the longest wait, half of MAXIMUM_SIMULATED_DELAY of lag and of jitter
 * plus REORDER_HOLD (10.075 s, about 605 ticks), it leaves 13 datagrams a
 * tick, where a server receives at most MAXIMUM_PLAYERS inputs a tick, 8 when
 * each is copied.
 */
constexpr std::size_t MAXIMUM_DATAGRAMS_IN_FLIGHT = 8192;

/**
 * @brief Seeds of the draws for the datagrams a program sends and for those
 * it receives. They are fixed, so two runs that hand over the same datagrams
 * at the same times lose and delay the same ones, and they differ, so the two
 * directions do not lose in step.
 */
constexpr std::uint32_t SENT_DATAGRAMS_SEED = 1;
constexpr std::uint32_t RECEIVED_DATAGRAMS_SEED = 2;

}  // namespace rtype::network

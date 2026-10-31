#pragma once

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
 * @brief Seeds of the draws for the datagrams a program sends and for those
 * it receives. They are fixed, so two runs that hand over the same datagrams
 * at the same times lose and delay the same ones, and they differ, so the two
 * directions do not lose in step.
 */
constexpr std::uint32_t SENT_DATAGRAMS_SEED = 1;
constexpr std::uint32_t RECEIVED_DATAGRAMS_SEED = 2;

}  // namespace rtype::network

#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/** @brief Ticks played over the lossy network: ten seconds. */
constexpr std::uint32_t CONVERGENCE_TICKS = 600;

/** @brief The datagrams the network loses: one in three, and thirty in a row
 * from the hundredth, about a second and a half of states. */
constexpr std::size_t LOSS_PERIOD = 3;
constexpr std::size_t LOSS_PHASE = 1;
constexpr std::size_t OUTAGE_FIRST_DATAGRAM = 100;
constexpr std::size_t OUTAGE_LENGTH = 30;

/** @brief The least states the client must rebuild from a delta over the ten
 * seconds, for the test to see deltas at work: a state in three is lost, so
 * most of the 200 arrive. */
constexpr std::size_t LEAST_STATES_FROM_DELTAS = 50;

}  // namespace rtype::server::testing

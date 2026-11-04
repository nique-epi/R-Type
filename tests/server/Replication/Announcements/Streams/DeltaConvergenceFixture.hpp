#pragma once

#include <cstddef>
#include <cstdint>
#include "DeltaStateSenderFixture.hpp"

namespace rtype::server::testing {

/** @brief What a game played over a network that loses datagrams came to. */
struct ConvergenceReport {
  std::size_t delivered;
  std::size_t rebuilt;
  std::size_t mismatching;
  std::size_t rebuiltFromDeltas;
  std::size_t rebuiltInFull;
};

/**
 * @brief A server and a client joined by a network that loses some datagrams:
 * the client rebuilds the states it receives with the WorldStateAssembler and
 * acknowledges what it accepted as a client does, and the server keeps the
 * acknowledgements.
 */
class DeltaConvergenceFixture : public DeltaStateSenderFixture {
 protected:
  /**
   * @brief Plays CONVERGENCE_TICKS ticks of a scene of enemies of which half
   * move at every tick, delivering each datagram to the client except those
   * the loss pattern drops.
   * @returns How many datagrams arrived, how many states the client rebuilt,
   * how many of those differ from the world at their tick, and how many were
   * written against an earlier state or in full.
   */
  [[nodiscard]] ConvergenceReport playOverALossyNetwork();
};

}  // namespace rtype::server::testing

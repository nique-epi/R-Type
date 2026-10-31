#pragma once

#include <chrono>

namespace rtype::network {

/**
 * @brief The defects of the network a program simulates, as its ping shows
 * them. NetworkConditions{} is a perfect network: nothing is simulated.
 *
 * Every datagram the program sends or receives crosses the simulated network
 * once, so a program alone gives a round trip the whole lag and jitter: half
 * on the way out, half on the way back. The shares count datagrams, in either
 * direction.
 */
struct NetworkConditions {
  /** @brief What the ping gains: each datagram waits half of it. */
  std::chrono::milliseconds lag{};
  /** @brief How much the ping may vary: each datagram waits up to half of it
   * more, drawn at random, and never arrives before the one handed over
   * before it. */
  std::chrono::milliseconds jitter{};
  /** @brief Percent of the datagrams that never arrive. */
  int lossPercent{};
  /** @brief Percent of the datagrams that arrive twice. */
  int duplicatePercent{};
  /** @brief Percent of the datagrams held REORDER_HOLD longer, so the ones
   * handed over just after them arrive first. */
  int reorderPercent{};

  bool operator==(const NetworkConditions&) const = default;
};

}  // namespace rtype::network

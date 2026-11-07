#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Event published when the health of a boss is news for the players:
 * its fight starts, a hit takes points from it, or it enrages. Its network
 * id, the points it has left out of the points it appeared with, and whether
 * it enraged. A hit that kills it publishes 0 before its destruction.
 */
struct BossHealthChanged {
  std::uint32_t networkId{};
  std::uint16_t points{};
  std::uint16_t full{};
  bool enraged{false};

  bool operator==(const BossHealthChanged&) const = default;
};

}  // namespace rtype::game

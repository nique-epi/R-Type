#pragma once

#include <cstdint>
#include <optional>

namespace rtype::game {

/** @brief A hit of a missile once DamageRules applied it: the two bodies by
 * their network ids, what it took, and what the target has left. */
struct AppliedHit {
  std::uint32_t targetNetworkId{0};
  std::uint32_t missileNetworkId{0};
  /** @brief The health points it took: 0 for a target without Health, or
   * armored. */
  int damage{0};
  /** @brief The health points the target has left; nothing for a target
   * without Health. */
  std::optional<int> healthLeft = std::nullopt;
  /** @brief Whether the hit destroyed the target. */
  bool destroyed{false};
};

}  // namespace rtype::game

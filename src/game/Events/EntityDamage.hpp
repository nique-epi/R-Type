#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Event published when an entity is hit and stays alive.
 */
struct EntityDamage {
  std::uint32_t networkId;

  bool operator==(const EntityDamage&) const = default;
};

}  // namespace rtype::game

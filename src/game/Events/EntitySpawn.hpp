#pragma once

#include <cstdint>
#include "EntityType.hpp"

namespace rtype::game {

/**
 * @brief Event published when an entity appears in the match. The network id
 * is the one of its NetworkIdentity, read when the event is published.
 */
struct EntitySpawn {
  std::uint32_t networkId;
  EntityType type;
  float x;
  float y;

  bool operator==(const EntitySpawn&) const = default;
};

}  // namespace rtype::game

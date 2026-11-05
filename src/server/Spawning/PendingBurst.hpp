#pragma once

#include <cstdint>
#include "Entity.hpp"
#include "Position.hpp"

namespace rtype::server {

/** @brief A projectile whose time to burst came: the entity, its network id,
 * where it is, and the shards it bursts into. */
struct PendingBurst {
  engine::Entity projectile{};
  std::uint32_t networkId{0};
  game::Position center{};
  std::uint8_t shardNumber{0};
  std::uint8_t count{0};
};

}  // namespace rtype::server

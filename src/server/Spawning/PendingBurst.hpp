#pragma once

#include <cstdint>
#include "Entity.hpp"
#include "Position.hpp"
#include "ShardBurst.hpp"

namespace rtype::server {

/** @brief A projectile whose time to burst came: the entity, its network id,
 * where it is, and its ShardBurst. */
struct PendingBurst {
  engine::Entity projectile{};
  std::uint32_t networkId{0};
  game::Position center{};
  game::ShardBurst burst{};
};

}  // namespace rtype::server

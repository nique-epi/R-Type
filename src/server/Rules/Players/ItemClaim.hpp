#pragma once

#include <cstdint>
#include "Entity.hpp"

namespace rtype::server {

/** @brief A pilot who touched an item during a tick: the item, its network
 * id, the pilot, and the network id of the ship that touched it. */
struct ItemClaim {
  engine::Entity item{};
  std::uint32_t itemId{0};
  std::uint8_t playerNumber{0};
  std::uint32_t shipId{0};
};

}  // namespace rtype::server

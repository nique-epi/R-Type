#pragma once

#include <vector>
#include "EntityAppearances.hpp"
#include "NumberedEntityType.hpp"
#include "ShipGun.hpp"

namespace rtype::client {

/** @brief What a client draws a server with: the appearance and the number
 * of each of its entity types, and the gun of its ships. */
struct PackContent {
  EntityAppearances appearances;
  std::vector<game::NumberedEntityType> types;
  game::ShipGun gun;
};

}  // namespace rtype::client

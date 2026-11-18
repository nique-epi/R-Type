#pragma once

#include <memory>
#include <vector>
#include "BackgroundCatalog.hpp"
#include "EntityAppearances.hpp"
#include "NumberedEntityType.hpp"
#include "ShipGun.hpp"

namespace rtype::client {

/** @brief What a client draws a server with: the appearance and the number
 * of each of its entity types, the gun of its ships, and the backgrounds its
 * resources bring, nullptr when they bring none. */
struct PackContent {
  EntityAppearances appearances;
  std::vector<game::NumberedEntityType> types;
  game::ShipGun gun;
  std::shared_ptr<const BackgroundCatalog> backgrounds;
};

}  // namespace rtype::client

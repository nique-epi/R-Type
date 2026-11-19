#pragma once

#include <filesystem>
#include <memory>
#include <vector>
#include "BackgroundCatalog.hpp"
#include "EntityAppearances.hpp"
#include "NumberedEntityType.hpp"
#include "ShipGun.hpp"

namespace rtype::client {

/** @brief What a client draws a server with: the appearance and the number
 * of each of its entity types, the gun of its ships, the backgrounds its
 * resources bring, nullptr when they bring none, and the folder its files
 * were written below. */
struct PackContent {
  EntityAppearances appearances;
  std::vector<game::NumberedEntityType> types;
  game::ShipGun gun;
  std::shared_ptr<const BackgroundCatalog> backgrounds;
  std::filesystem::path folder;
};

}  // namespace rtype::client

#pragma once

#include <vector>
#include "EntityAppearances.hpp"
#include "NumberedEntityType.hpp"

namespace rtype::client {

/** @brief What a client draws a server with: the appearance and the number
 * of each of its entity types. */
struct PackContent {
  EntityAppearances appearances;
  std::vector<game::NumberedEntityType> types;
};

}  // namespace rtype::client

#pragma once

#include <string>
#include "EntitySide.hpp"

namespace rtype::network {

/**
 * @brief One entity type of the table ConnectionAccepted carries: its side and
 * the name of its sheet. Its number is its position in the table, from 1.
 */
struct EntityTypeEntry {
  EntitySide side{EntitySide::Player};
  std::string name;

  bool operator==(const EntityTypeEntry&) const = default;
};

}  // namespace rtype::network

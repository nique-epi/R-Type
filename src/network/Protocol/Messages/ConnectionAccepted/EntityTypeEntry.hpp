#pragma once

#include <cstdint>
#include <string>
#include "EntitySide.hpp"

namespace rtype::network {

/**
 * @brief One entity type of the table ConnectionAccepted carries: its side, the
 * name of its sheet and the size of its box, in whole logical units, so the
 * client can tell where its own missile touches. Its number is its position in
 * the table, from 1.
 */
struct EntityTypeEntry {
  EntitySide side{EntitySide::Player};
  std::string name;
  std::uint16_t hitboxWidth{};
  std::uint16_t hitboxHeight{};

  bool operator==(const EntityTypeEntry&) const = default;
};

}  // namespace rtype::network

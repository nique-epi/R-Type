#pragma once

#include <cstdint>
#include <vector>
#include "WorldState.hpp"

namespace rtype::network {

/**
 * @brief What differs between two lists of entities: the entities of the later
 * list that are new or placed otherwise, and the ids the earlier list has and
 * the later one has not.
 */
struct StateDelta {
  std::vector<EntityState> changed;
  std::vector<std::uint32_t> removed;

  bool operator==(const StateDelta&) const = default;
};

}  // namespace rtype::network

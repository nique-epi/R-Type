#include "WorldStateDelta.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <span>
#include <vector>
#include "StateDelta.hpp"
#include "WireTypes.hpp"
#include "WorldState.hpp"

namespace rtype::network {

namespace {

/** @returns Whether a receiver reads the same entity from @p base and from
 * @p current: its type, its position on the wire and its look. */
bool readAlike(const EntityState& base, const EntityState& current) {
  return base.entityType == current.entityType &&
         wireStepsOf(base.x) == wireStepsOf(current.x) &&
         wireStepsOf(base.y) == wireStepsOf(current.y) &&
         base.look == current.look;
}

bool byEntityId(const EntityState& left, const EntityState& right) {
  return left.entityId < right.entityId;
}

bool sameEntityId(const EntityState& left, const EntityState& right) {
  return left.entityId == right.entityId;
}

}  // namespace

StateDelta diffStates(std::span<const EntityState> base,
                      std::span<const EntityState> current) {
  StateDelta delta;
  std::size_t baseIndex = 0;
  std::size_t currentIndex = 0;
  while (baseIndex < base.size() || currentIndex < current.size()) {
    const bool baseOnly =
        currentIndex == current.size() ||
        (baseIndex < base.size() &&
         base[baseIndex].entityId < current[currentIndex].entityId);
    if (baseOnly) {
      delta.removed.push_back(base[baseIndex++].entityId);
    } else if (baseIndex == base.size() ||
               current[currentIndex].entityId < base[baseIndex].entityId) {
      delta.changed.push_back(current[currentIndex++]);
    } else {
      if (!readAlike(base[baseIndex], current[currentIndex])) {
        delta.changed.push_back(current[currentIndex]);
      }
      ++baseIndex;
      ++currentIndex;
    }
  }
  return delta;
}

std::vector<EntityState> applyDelta(std::span<const EntityState> base,
                                    const StateDelta& delta) {
  std::vector<std::uint32_t> removed = delta.removed;
  std::ranges::sort(removed);
  std::vector<EntityState> entities = delta.changed;
  entities.reserve(base.size() + delta.changed.size());
  std::ranges::copy_if(base, std::back_inserter(entities),
                       [&removed](const EntityState& entity) {
                         return !std::ranges::binary_search(removed,
                                                            entity.entityId);
                       });
  std::ranges::stable_sort(entities, byEntityId);
  const auto [firstDuplicate, end] =
      std::ranges::unique(entities, sameEntityId);
  entities.erase(firstDuplicate, end);
  return entities;
}

}  // namespace rtype::network

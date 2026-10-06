#include "EntityIndexMap.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>

namespace rtype::engine {

std::optional<std::size_t> EntityIndexMap::find(
    std::uint32_t entityIndex) const {
  if (entityIndex >= positionByEntityIndex_.size() ||
      positionByEntityIndex_[entityIndex] == NO_POSITION) {
    return std::nullopt;
  }
  return positionByEntityIndex_[entityIndex];
}

void EntityIndexMap::append(std::uint32_t entityIndex) {
  if (find(entityIndex).has_value()) {
    return;
  }
  if (entityIndex >= positionByEntityIndex_.size()) {
    positionByEntityIndex_.resize(static_cast<std::size_t>(entityIndex) + 1,
                                  NO_POSITION);
  }
  positionByEntityIndex_[entityIndex] = entityIndices_.size();
  entityIndices_.push_back(entityIndex);
}

std::optional<std::size_t> EntityIndexMap::erase(std::uint32_t entityIndex) {
  const std::optional<std::size_t> position = find(entityIndex);
  if (!position.has_value()) {
    return std::nullopt;
  }
  const std::uint32_t movedEntityIndex = entityIndices_.back();
  entityIndices_[*position] = movedEntityIndex;
  positionByEntityIndex_[movedEntityIndex] = *position;
  entityIndices_.pop_back();
  positionByEntityIndex_[entityIndex] = NO_POSITION;
  return position;
}

std::size_t EntityIndexMap::size() const { return entityIndices_.size(); }

std::uint32_t EntityIndexMap::entityIndexAt(std::size_t position) const {
  return entityIndices_[position];
}

}  // namespace rtype::engine

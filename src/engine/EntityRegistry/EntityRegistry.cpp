#include "EntityRegistry.hpp"
#include <cstddef>
#include <cstdint>
#include "Entity.hpp"

namespace rtype::engine {

Entity EntityRegistry::create() {
  std::uint32_t index = 0;
  if (freeIndices_.empty()) {
    index = static_cast<std::uint32_t>(generations_.size());
    generations_.push_back(0);
  } else {
    index = freeIndices_.back();
    freeIndices_.pop_back();
  }
  return Entity{.index = index, .generation = generations_[index]};
}

void EntityRegistry::destroy(Entity entity) {
  if (!isAlive(entity)) {
    return;
  }
  ++generations_[entity.index];
  freeIndices_.push_back(entity.index);
}

bool EntityRegistry::isAlive(Entity entity) const {
  return entity.index < generations_.size() &&
         generations_[entity.index] == entity.generation;
}

Entity EntityRegistry::entityAt(std::uint32_t entityIndex) const {
  return Entity{.index = entityIndex, .generation = generations_[entityIndex]};
}

std::size_t EntityRegistry::size() const {
  return generations_.size() - freeIndices_.size();
}

}  // namespace rtype::engine

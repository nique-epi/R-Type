#include "ComponentRegistry.hpp"
#include <cstdint>
#include "Entity.hpp"
#include "EntityRegistry.hpp"

namespace rtype::engine {

ComponentRegistry::ComponentRegistry(EntityRegistry& entities)
    : entities_(entities) {}

void ComponentRegistry::destroy(Entity entity) {
  if (!entities_.isAlive(entity)) {
    return;
  }
  if (iterationDepth_ > 0) {
    pendingDestructionIndices_.insert(entity.index);
    return;
  }
  destroyNow(entity);
}

bool ComponentRegistry::isPendingDestruction(std::uint32_t entityIndex) const {
  return pendingDestructionIndices_.contains(entityIndex);
}

void ComponentRegistry::destroyNow(Entity entity) {
  for (auto& [type, storage] : storages_) {
    storage->erase(entity.index);
  }
  entities_.destroy(entity);
}

void ComponentRegistry::flushPendingDestructions() {
  for (const std::uint32_t entityIndex : pendingDestructionIndices_) {
    destroyNow(entities_.entityAt(entityIndex));
  }
  pendingDestructionIndices_.clear();
}

ComponentRegistry::IterationScope::IterationScope(ComponentRegistry& registry)
    : registry_(registry) {
  ++registry_.iterationDepth_;
}

ComponentRegistry::IterationScope::~IterationScope() {
  --registry_.iterationDepth_;
  if (registry_.iterationDepth_ == 0) {
    registry_.flushPendingDestructions();
  }
}

}  // namespace rtype::engine

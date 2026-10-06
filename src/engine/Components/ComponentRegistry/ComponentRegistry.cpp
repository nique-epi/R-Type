#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"

namespace rtype::engine {

ComponentRegistry::ComponentRegistry(EntityRegistry& entities)
    : entities_(entities) {}

void ComponentRegistry::destroy(Entity entity) {
  if (!entities_.isAlive(entity)) {
    return;
  }
  for (auto& [type, storage] : storages_) {
    storage->erase(entity.index);
  }
  entities_.destroy(entity);
}

}  // namespace rtype::engine

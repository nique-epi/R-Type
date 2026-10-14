#include "World.hpp"
#include <cstddef>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"

namespace rtype::game {

engine::Entity World::spawnPlayer() { return registry_.create(); }

std::size_t World::entityCount() const { return registry_.size(); }

engine::ComponentRegistry& World::components() { return components_; }

}  // namespace rtype::game

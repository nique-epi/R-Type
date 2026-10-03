#include "World.hpp"

namespace rtype::game {

engine::Entity World::spawnPlayer() { return registry_.create(); }

std::size_t World::entityCount() const { return registry_.size(); }

}  // namespace rtype::game

#include "World.hpp"
#include <cstddef>
#include <cstdint>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "NetworkIdentity.hpp"
#include "Position.hpp"

namespace rtype::game {

std::uint32_t World::spawnPlayer(Position position) {
  const std::uint32_t networkId = networkIds_.allocate();
  const engine::Entity ship = registry_.create();
  components_.add(ship, position);
  components_.add(ship, EntityType::Player);
  components_.add(ship, NetworkIdentity{.networkId = networkId});
  return networkId;
}

std::size_t World::entityCount() const { return registry_.size(); }

engine::ComponentRegistry& World::components() { return components_; }

}  // namespace rtype::game

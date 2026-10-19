#include "World.hpp"
#include <cstddef>
#include <cstdint>
#include "BydoKind.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "MovementPattern.hpp"
#include "NetworkIdentity.hpp"
#include "PatternConstants.hpp"
#include "Position.hpp"
#include "Velocity.hpp"

namespace rtype::game {

std::uint32_t World::spawnPlayer(Position position) {
  const std::uint32_t networkId = networkIds_.allocate();
  const engine::Entity ship = registry_.create();
  components_.add(ship, position);
  components_.add(ship, EntityType::Player);
  components_.add(ship, NetworkIdentity{.networkId = networkId});
  return networkId;
}

std::uint32_t World::spawnBydo(Position position, BydoKind kind) {
  const std::uint32_t networkId = networkIds_.allocate();
  const engine::Entity bydo = registry_.create();
  components_.add(bydo, position);
  components_.add(bydo, Velocity{.x = -BYDO_SPEED, .y = 0.0F});
  components_.add(bydo, MovementPattern{.shape = patternShapeOf(kind),
                                        .originY = position.y,
                                        .ageSeconds = 0.0F});
  components_.add(bydo, EntityType::Enemy);
  components_.add(bydo, NetworkIdentity{.networkId = networkId});
  return networkId;
}

std::size_t World::entityCount() const { return registry_.size(); }

engine::ComponentRegistry& World::components() { return components_; }

}  // namespace rtype::game

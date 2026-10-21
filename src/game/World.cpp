#include "World.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "BydoKind.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "Health.hpp"
#include "HealthConstants.hpp"
#include "MovementPattern.hpp"
#include "NetworkIdentity.hpp"
#include "PatternConstants.hpp"
#include "Position.hpp"
#include "ScoreValue.hpp"
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

bool World::removePlayer(std::uint32_t shipNetworkId) {
  const std::optional<engine::Entity> ship = playerShipCarrying(shipNetworkId);
  if (!ship.has_value()) {
    return false;
  }
  components_.destroy(*ship);
  return true;
}

std::optional<engine::Entity> World::playerShipCarrying(
    std::uint32_t networkId) {
  std::optional<engine::Entity> ship;
  components_.forEach<NetworkIdentity, EntityType>(
      [&ship, networkId](engine::Entity entity, const NetworkIdentity& identity,
                         const EntityType& type) {
        if (type == EntityType::Player && identity.networkId == networkId) {
          ship = entity;
        }
      });
  return ship;
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
  components_.add(bydo, Health{.points = BYDO_HEALTH});
  components_.add(bydo, ScoreValue{.points = pointsOf(kind)});
  components_.add(bydo, NetworkIdentity{.networkId = networkId});
  return networkId;
}

std::size_t World::entityCount() const { return registry_.size(); }

engine::ComponentRegistry& World::components() { return components_; }

}  // namespace rtype::game

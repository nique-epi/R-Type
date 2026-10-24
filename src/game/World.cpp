#include "World.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "BydoKind.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntitySizeConstants.hpp"
#include "EntityType.hpp"
#include "Health.hpp"
#include "HealthConstants.hpp"
#include "MissileConstants.hpp"
#include "MissileOwner.hpp"
#include "MovementPattern.hpp"
#include "NetworkEntityTable.hpp"
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
  shipByNetworkId_.bind(networkId, ship);
  return networkId;
}

bool World::removePlayer(std::uint32_t shipNetworkId) {
  const std::optional<engine::Entity> ship =
      shipByNetworkId_.find(shipNetworkId);
  if (!ship.has_value()) {
    return false;
  }
  shipByNetworkId_.unbind(shipNetworkId);
  components_.destroy(*ship);
  return true;
}

std::uint32_t World::spawnBydo(Position position, BydoKind kind) {
  const std::uint32_t networkId = networkIds_.allocate();
  const engine::Entity bydo = registry_.create();
  components_.add(bydo, position);
  components_.add(bydo, Velocity{.x = -BYDO_SPEED, .y = 0.0F});
  components_.add(bydo, MovementPattern{.shape = patternShapeOf(kind),
                                        .originY = position.y,
                                        .ageSeconds = 0.0F});
  components_.add(bydo,
                  CollisionBox{.width = BYDO_WIDTH, .height = BYDO_HEIGHT});
  components_.add(bydo, EntityType::Enemy);
  components_.add(bydo, Health{.points = BYDO_HEALTH});
  components_.add(bydo, ScoreValue{.points = pointsOf(kind)});
  components_.add(bydo, NetworkIdentity{.networkId = networkId});
  return networkId;
}

std::uint32_t World::spawnPlayerMissile(Position position,
                                        std::uint8_t playerNumber) {
  const std::uint32_t networkId = networkIds_.allocate();
  const engine::Entity missile = registry_.create();
  components_.add(missile, position);
  components_.add(missile, Velocity{.x = PLAYER_MISSILE_SPEED, .y = 0.0F});
  components_.add(missile, CollisionBox{.width = PLAYER_MISSILE_WIDTH,
                                        .height = PLAYER_MISSILE_HEIGHT});
  components_.add(missile, EntityType::PlayerMissile);
  components_.add(missile, MissileOwner{.playerNumber = playerNumber});
  components_.add(missile, NetworkIdentity{.networkId = networkId});
  return networkId;
}

std::size_t World::entityCount() const { return registry_.size(); }

engine::ComponentRegistry& World::components() { return components_; }

Position* World::shipPositionOf(std::uint32_t shipId) {
  const std::optional<engine::Entity> ship = shipByNetworkId_.find(shipId);
  if (!ship.has_value()) {
    return nullptr;
  }
  return components_.get<Position>(*ship);
}

}  // namespace rtype::game

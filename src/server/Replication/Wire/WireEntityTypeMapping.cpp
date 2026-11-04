#include "WireEntityTypeMapping.hpp"
#include <optional>
#include "EnemyKind.hpp"
#include "EntityType.hpp"
#include "WorldState.hpp"

namespace rtype::server {

namespace {

network::WireEntityType toWireEnemyType(std::optional<game::EnemyKind> kind) {
  if (!kind.has_value()) {
    return network::WireEntityType::Bydo;
  }
  switch (*kind) {
    case game::EnemyKind::Drifter:
    case game::EnemyKind::Weaver:
      return network::WireEntityType::Bydo;
    case game::EnemyKind::Voltjelly:
      return network::WireEntityType::Voltjelly;
    case game::EnemyKind::Reaper:
      return network::WireEntityType::Reaper;
    case game::EnemyKind::Bydomancer:
      return network::WireEntityType::Bydomancer;
    case game::EnemyKind::Drillback:
      return network::WireEntityType::Drillback;
    case game::EnemyKind::BydoTyrant:
      return network::WireEntityType::BydoTyrant;
  }
  return network::WireEntityType::Bydo;
}

}  // namespace

network::WireEntityType toWireEntityType(game::EntityType type,
                                         std::optional<game::EnemyKind> kind) {
  switch (type) {
    case game::EntityType::Player:
      return network::WireEntityType::PlayerShip;
    case game::EntityType::Enemy:
      return toWireEnemyType(kind);
    case game::EntityType::PlayerMissile:
      return network::WireEntityType::PlayerMissile;
    case game::EntityType::EnemyMissile:
      return network::WireEntityType::EnemyMissile;
  }
  return network::WireEntityType::PlayerShip;
}

}  // namespace rtype::server

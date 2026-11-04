#include "WireReading.hpp"
#include <optional>
#include "EnemyKind.hpp"
#include "EntityType.hpp"
#include "WorldState.hpp"

namespace rtype::client {

namespace {

AnnouncedEntity enemyOf(game::EnemyKind kind) {
  return AnnouncedEntity{.type = game::EntityType::Enemy, .kind = kind};
}

AnnouncedEntity plain(game::EntityType type) {
  return AnnouncedEntity{.type = type, .kind = std::nullopt};
}

}  // namespace

std::optional<AnnouncedEntity> announcedEntityOf(
    network::WireEntityType wireType) {
  switch (wireType) {
    case network::WireEntityType::PlayerShip:
      return plain(game::EntityType::Player);
    case network::WireEntityType::PlayerMissile:
      return plain(game::EntityType::PlayerMissile);
    case network::WireEntityType::Bydo:
      return enemyOf(game::EnemyKind::Drifter);
    case network::WireEntityType::Voltjelly:
      return enemyOf(game::EnemyKind::Voltjelly);
    case network::WireEntityType::Reaper:
      return enemyOf(game::EnemyKind::Reaper);
    case network::WireEntityType::Bydomancer:
      return enemyOf(game::EnemyKind::Bydomancer);
    case network::WireEntityType::Drillback:
      return enemyOf(game::EnemyKind::Drillback);
    case network::WireEntityType::BydoTyrant:
      return enemyOf(game::EnemyKind::BydoTyrant);
    case network::WireEntityType::EnemyMissile:
    case network::WireEntityType::RuneOrb:
    case network::WireEntityType::Bone:
    case network::WireEntityType::Beam:
      return plain(game::EntityType::EnemyMissile);
  }
  return std::nullopt;
}

}  // namespace rtype::client

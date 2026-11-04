#include "WireReading.hpp"
#include <optional>
#include "ActionKey.hpp"
#include "EnemyAction.hpp"
#include "EnemyKind.hpp"
#include "EnemyPhase.hpp"
#include "EntityType.hpp"
#include "WorldState.hpp"

namespace rtype::client {

namespace {

/** @returns The kind of enemy a protocol type names, or nothing when it names
 * something else. A Bydo is read as a Drifter: it is drawn alike a Weaver. */
std::optional<game::EnemyKind> enemyKindOf(network::WireEntityType wireType) {
  switch (wireType) {
    case network::WireEntityType::Bydo:
      return game::EnemyKind::Drifter;
    case network::WireEntityType::Voltjelly:
      return game::EnemyKind::Voltjelly;
    case network::WireEntityType::Reaper:
      return game::EnemyKind::Reaper;
    case network::WireEntityType::Bydomancer:
      return game::EnemyKind::Bydomancer;
    case network::WireEntityType::Drillback:
      return game::EnemyKind::Drillback;
    case network::WireEntityType::BydoTyrant:
      return game::EnemyKind::BydoTyrant;
    case network::WireEntityType::PlayerShip:
    case network::WireEntityType::PlayerMissile:
    case network::WireEntityType::EnemyMissile:
    case network::WireEntityType::RuneOrb:
    case network::WireEntityType::Bone:
    case network::WireEntityType::Beam:
      return std::nullopt;
  }
  return std::nullopt;
}

/** @returns The game type of a protocol type that is not an enemy: a ship, a
 * missile of a player, or a projectile of an enemy, which the projectiles of
 * the new enemies are drawn as until their own pull request. Nothing for an
 * enemy. */
std::optional<game::EntityType> otherTypeOf(network::WireEntityType wireType) {
  switch (wireType) {
    case network::WireEntityType::PlayerShip:
      return game::EntityType::Player;
    case network::WireEntityType::PlayerMissile:
      return game::EntityType::PlayerMissile;
    case network::WireEntityType::EnemyMissile:
    case network::WireEntityType::RuneOrb:
    case network::WireEntityType::Bone:
    case network::WireEntityType::Beam:
      return game::EntityType::EnemyMissile;
    case network::WireEntityType::Bydo:
    case network::WireEntityType::Voltjelly:
    case network::WireEntityType::Reaper:
    case network::WireEntityType::Bydomancer:
    case network::WireEntityType::Drillback:
    case network::WireEntityType::BydoTyrant:
      return std::nullopt;
  }
  return std::nullopt;
}

}  // namespace

std::optional<AnnouncedEntity> announcedEntityOf(
    network::WireEntityType wireType) {
  if (const std::optional<game::EnemyKind> kind = enemyKindOf(wireType)) {
    return AnnouncedEntity{.type = game::EntityType::Enemy, .kind = kind};
  }
  if (const std::optional<game::EntityType> type = otherTypeOf(wireType)) {
    return AnnouncedEntity{.type = *type, .kind = std::nullopt};
  }
  return std::nullopt;
}

game::ActionKey actionKeyOf(const network::EnemyAction& message) {
  switch (message.phase) {
    case network::EnemyActionPhase::Signal:
      return {.attack = message.attack, .phase = game::EnemyPhase::Signal};
    case network::EnemyActionPhase::Strike:
      return {.attack = message.attack, .phase = game::EnemyPhase::Strike};
    case network::EnemyActionPhase::Recover:
      return {.attack = message.attack, .phase = game::EnemyPhase::Recover};
    case network::EnemyActionPhase::Enraged:
      return {.attack = message.attack, .phase = game::EnemyPhase::Enraged};
  }
  return {.attack = message.attack, .phase = game::EnemyPhase::Signal};
}

}  // namespace rtype::client

#include "WeaponSystem.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EnemyMissileStartPosition.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "Gun.hpp"
#include "Intent.hpp"
#include "MissileStartPosition.hpp"
#include "NetworkIdentity.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "WeaponShot.hpp"
#include "WeaponSystemConstants.hpp"
#include "WireEntityTypeMapping.hpp"
#include "World.hpp"
#include "WorldState.hpp"

namespace rtype::server {

namespace {

/**
 * @brief Counts one tick off @p gun, when it has some left.
 * @returns Whether @p gun is ready to fire after that tick.
 */
bool countsDownToReady(game::Gun& gun) {
  if (gun.ticksUntilReady > 0) {
    --gun.ticksUntilReady;
  }
  return gun.ticksUntilReady == 0;
}

/**
 * @returns The EntitySpawned that announces the projectile @p projectileId of
 * type @p type, at @p position.
 */
network::EntitySpawned spawnAt(std::uint32_t projectileId,
                               game::EntityType type,
                               const game::Position& position) {
  return network::EntitySpawned{.entityId = projectileId,
                                .entityType = toWireEntityType(type),
                                .x = position.x,
                                .y = position.y};
}

}  // namespace

WeaponSystem::WeaponSystem(game::World& world, PlayerRoster& roster,
                           SendDatagram send)
    : world_(&world),
      roster_(&roster),
      send_(std::move(send)),
      logger_(std::string{WEAPON_SYSTEM_LOGGER_NAME}) {}

void WeaponSystem::onTick() {
  collectShots();
  for (const WeaponShot& shot : shots_) {
    fire(shot);
  }
}

void WeaponSystem::collectShots() {
  shots_.clear();
  engine::ComponentRegistry& components = world_->components();
  components.forEach<game::Gun, game::Intent>(
      [this, &components](engine::Entity body, game::Gun& gun,
                          const game::Intent& intent) {
        if (!countsDownToReady(gun) || !intent.shooting) {
          return;
        }
        const game::Position* const position =
            components.get<game::Position>(body);
        const game::NetworkIdentity* const identity =
            components.get<game::NetworkIdentity>(body);
        if (position == nullptr || identity == nullptr) {
          return;
        }
        gun.ticksUntilReady = gun.intervalTicks;
        shots_.push_back(WeaponShot{.muzzle = *position,
                                    .projectile = gun.projectile,
                                    .shooterId = identity->networkId});
      });
}

void WeaponSystem::fire(const WeaponShot& shot) {
  const std::optional<network::EntitySpawned> projectile =
      spawnProjectile(shot);
  if (projectile.has_value()) {
    announce(*projectile);
  }
}

std::optional<network::EntitySpawned> WeaponSystem::spawnProjectile(
    const WeaponShot& shot) {
  try {
    if (shot.projectile == game::EntityType::PlayerMissile) {
      const ConnectedPlayer* const owner = roster_->findByShip(shot.shooterId);
      if (owner == nullptr) {
        return std::nullopt;
      }
      const game::Position start = game::missileStartPosition(shot.muzzle);
      return spawnAt(world_->spawnPlayerMissile(start, owner->playerNumber),
                     shot.projectile, start);
    }
    const game::Position start = game::enemyMissileStartPosition(shot.muzzle);
    return spawnAt(world_->spawnEnemyMissile(start), shot.projectile, start);
  } catch (const engine::NetworkIdExhaustedException&) {
    logger_.warn("no projectile fired: every network id was given");
    return std::nullopt;
  }
}

void WeaponSystem::announce(const network::EntitySpawned& projectile) {
  for (ConnectedPlayer* const player : roster_->connected()) {
    sendEntitySpawned(send_, *player, projectile);
  }
}

}  // namespace rtype::server

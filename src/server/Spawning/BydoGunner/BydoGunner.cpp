#include "BydoGunner.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include "BydoGunnerConstants.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EnemyMissileStartPosition.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "Gun.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "World.hpp"
#include "WorldState.hpp"

namespace rtype::server {

namespace {

/**
 * @brief Counts one tick on @p gun.
 * @returns Whether its interval is over, the gun then starting again from 0.
 */
bool countsDownToAShot(game::Gun& gun) {
  ++gun.ticksSinceLastShot;
  if (gun.ticksSinceLastShot < ENEMY_SHOT_INTERVAL_TICKS) {
    return false;
  }
  gun.ticksSinceLastShot = 0;
  return true;
}

/**
 * @returns The EntitySpawned that announces the missile of an enemy @p
 * missileId, at @p position.
 */
network::EntitySpawned enemyMissileSpawnAt(std::uint32_t missileId,
                                           const game::Position& position) {
  return network::EntitySpawned{
      .entityId = missileId,
      .entityType = network::WireEntityType::EnemyMissile,
      .x = position.x,
      .y = position.y};
}

}  // namespace

BydoGunner::BydoGunner(game::World& world, PlayerRoster& roster,
                       SendDatagram send)
    : world_(&world),
      roster_(&roster),
      send_(std::move(send)),
      logger_(std::string{BYDO_GUNNER_LOGGER_NAME}) {}

void BydoGunner::onTick() {
  findTheBydosThatFire();
  for (const game::Position bydo : firingBydos_) {
    fire(bydo);
  }
}

void BydoGunner::findTheBydosThatFire() {
  firingBydos_.clear();
  world_->components().forEach<game::Position, game::Gun>(
      [this](engine::Entity /*bydo*/, const game::Position& position,
             game::Gun& gun) {
        if (countsDownToAShot(gun)) {
          firingBydos_.push_back(position);
        }
      });
}

void BydoGunner::fire(game::Position bydo) {
  const std::optional<network::EntitySpawned> missile = spawnMissile(bydo);
  if (missile.has_value()) {
    announce(*missile);
  }
}

std::optional<network::EntitySpawned> BydoGunner::spawnMissile(
    game::Position bydo) {
  const game::Position start = game::enemyMissileStartPosition(bydo);
  try {
    return enemyMissileSpawnAt(world_->spawnEnemyMissile(start), start);
  } catch (const engine::NetworkIdExhaustedException&) {
    logger_.warn("no missile of a Bydo fired: every network id was given");
    return std::nullopt;
  }
}

void BydoGunner::announce(const network::EntitySpawned& missile) {
  for (ConnectedPlayer* const player : roster_->connected()) {
    sendEntitySpawned(send_, *player, missile);
  }
}

}  // namespace rtype::server

#include "MissileLauncher.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "EngineException.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "MissileLauncherConstants.hpp"
#include "MissileStartPosition.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "World.hpp"
#include "WorldState.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The EntitySpawned that announces the missile @p missileId, at
 * @p position.
 */
network::EntitySpawned missileSpawnAt(std::uint32_t missileId,
                                      const game::Position& position) {
  return network::EntitySpawned{
      .entityId = missileId,
      .entityType = network::WireEntityType::PlayerMissile,
      .x = position.x,
      .y = position.y};
}

}  // namespace

MissileLauncher::MissileLauncher(game::World& world, PlayerRoster& roster,
                                 SendDatagram send)
    : world_(&world),
      roster_(&roster),
      send_(std::move(send)),
      logger_(std::string{MISSILE_LAUNCHER_LOGGER_NAME}) {}

void MissileLauncher::onTick(const ConnectedPlayers& shooters) {
  for (const ConnectedPlayer* const shooter : shooters) {
    const std::optional<game::Position> ship = firingShipOf(*shooter);
    if (ship.has_value()) {
      fire(*shooter, *ship);
    }
  }
  ++ticksRun_;
}

std::optional<game::Position> MissileLauncher::firingShipOf(
    const ConnectedPlayer& shooter) const {
  if (!mayFire(shooter.shipEntityId)) {
    return std::nullopt;
  }
  const game::Position* const ship =
      world_->shipPositionOf(shooter.shipEntityId);
  if (ship == nullptr) {
    return std::nullopt;
  }
  return *ship;
}

bool MissileLauncher::mayFire(std::uint32_t shipId) const {
  const auto nextShotTick = nextShotTickByShip_.find(shipId);
  return nextShotTick == nextShotTickByShip_.end() ||
         ticksRun_ >= nextShotTick->second;
}

void MissileLauncher::fire(const ConnectedPlayer& shooter,
                           game::Position ship) {
  const std::optional<network::EntitySpawned> missile =
      spawnMissile(shooter, ship);
  if (!missile.has_value()) {
    return;
  }
  nextShotTickByShip_[shooter.shipEntityId] = ticksRun_ + SHOT_INTERVAL_TICKS;
  announce(*missile);
}

std::optional<network::EntitySpawned> MissileLauncher::spawnMissile(
    const ConnectedPlayer& shooter, game::Position ship) {
  const game::Position start = game::missileStartPosition(ship);
  try {
    return missileSpawnAt(
        world_->spawnPlayerMissile(start, shooter.playerNumber), start);
  } catch (const engine::NetworkIdExhaustedException&) {
    logger_.warn("no missile fired: every network id was given");
    return std::nullopt;
  }
}

void MissileLauncher::announce(const network::EntitySpawned& missile) {
  for (ConnectedPlayer* const player : roster_->connected()) {
    sendEntitySpawned(send_, *player, missile);
  }
}

}  // namespace rtype::server

#include "WeaponSystem.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include "BodyPlacement.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "EntityTypeNumber.hpp"
#include "EventBus.hpp"
#include "Gun.hpp"
#include "Intent.hpp"
#include "Muzzle.hpp"
#include "NetworkIdentity.hpp"
#include "PendingBurst.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "ProjectileStartPosition.hpp"
#include "ShardBurst.hpp"
#include "WeaponShot.hpp"
#include "WeaponSystemConstants.hpp"
#include "WeaponSystemParts.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/** @brief A projectile about to appear: its sheet, where it starts, and the
 * player it belongs to, for a missile of a player. */
struct ProjectileLaunch {
  std::reference_wrapper<const game::EntitySheet> sheet;
  game::Position start{};
  std::optional<std::uint8_t> owner;
};

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
 * @returns The shot of the body @p body of @p components whose Gun is @p gun:
 * the body, what it fires and under which network id; nothing for a body that
 * lacks a Position, a CollisionBox, an EntityType or a NetworkIdentity.
 */
std::optional<WeaponShot> shotOf(engine::ComponentRegistry& components,
                                 engine::Entity body, const game::Gun& gun) {
  const auto* const position = components.get<game::Position>(body);
  const auto* const box = components.get<game::CollisionBox>(body);
  const auto* const side = components.get<game::EntityType>(body);
  const auto* const identity = components.get<game::NetworkIdentity>(body);
  if (position == nullptr || box == nullptr || side == nullptr ||
      identity == nullptr) {
    return std::nullopt;
  }
  return WeaponShot{
      .muzzle = game::Muzzle{.center = *position, .box = *box, .side = *side},
      .projectileNumber = gun.projectileNumber,
      .shooterId = identity->networkId};
}

/**
 * @returns How the projectile of @p shot appears: its sheet among @p types,
 * its start in front of the shooter, and, for a missile of a player, the
 * player of @p roster whose ship fired it. Nothing when no sheet has the
 * number of the gun, or when a missile of a player was fired by a ship of no
 * connected player.
 */
std::optional<ProjectileLaunch> launchOf(const WeaponShot& shot,
                                         const game::EntitySheets& types,
                                         PlayerRoster& roster) {
  const game::EntitySheet* const sheet = types.numbered(shot.projectileNumber);
  if (sheet == nullptr) {
    return std::nullopt;
  }
  std::optional<std::uint8_t> owner;
  if (sheet->side == game::EntityType::PlayerMissile) {
    const ConnectedPlayer* const shooter = roster.findByShip(shot.shooterId);
    if (shooter == nullptr) {
      return std::nullopt;
    }
    owner = shooter->playerNumber;
  }
  return ProjectileLaunch{
      .sheet = *sheet,
      .start = game::projectileStartPosition(shot.muzzle, sheet->hitbox),
      .owner = owner};
}

/**
 * @returns The EntitySpawned of the projectile made from @p launch, which was
 * given @p networkId.
 */
network::EntitySpawned spawnOf(std::uint32_t networkId,
                               const ProjectileLaunch& launch) {
  return entitySpawnedOf(
      networkId, game::EntityTypeNumber{.number = launch.sheet.get().number},
      launch.start);
}

/** @returns Where the shard @p index of @p pending appears and steers from:
 * SHARD_START_DISTANCE from the center of the projectile, along the direction
 * of that shard, y growing downwards. */
game::BodyPlacement shardPlacementOf(const PendingBurst& pending,
                                     std::size_t index) {
  const float angle = FIRST_SHARD_ANGLE_RADIANS +
                      (SHARD_SPREAD_RADIANS * static_cast<float>(index) /
                       static_cast<float>(pending.count));
  const game::Position start{
      .x = pending.center.x + (SHARD_START_DISTANCE * std::cos(angle)),
      .y = pending.center.y - (SHARD_START_DISTANCE * std::sin(angle))};
  return game::BodyPlacement{.position = start, .origin = pending.center};
}

}  // namespace

WeaponSystem::WeaponSystem(WeaponSystemParts parts)
    : world_(&parts.world.get()),
      roster_(&parts.roster.get()),
      send_(std::move(parts.send)),
      types_(&parts.types.get()),
      events_(&parts.events.get()),
      logger_(std::string{WEAPON_SYSTEM_LOGGER_NAME}) {}

void WeaponSystem::onTick() {
  collectShots();
  for (const WeaponShot& shot : shots_) {
    fire(shot);
  }
  collectBursts();
  for (const PendingBurst& pending : bursts_) {
    burst(pending);
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
        const std::optional<WeaponShot> shot = shotOf(components, body, gun);
        if (!shot.has_value()) {
          return;
        }
        gun.ticksUntilReady = gun.intervalTicks;
        shots_.push_back(*shot);
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
  const std::optional<ProjectileLaunch> launch =
      launchOf(shot, *types_, *roster_);
  if (!launch.has_value()) {
    return std::nullopt;
  }
  try {
    return spawnOf(
        world_->spawn(launch->sheet.get(), launch->start, launch->owner),
        *launch);
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

void WeaponSystem::collectBursts() {
  bursts_.clear();
  engine::ComponentRegistry& components = world_->components();
  components.forEach<game::ShardBurst, game::Position>(
      [this, &components](engine::Entity projectile, game::ShardBurst& burst,
                          const game::Position& position) {
        if (burst.ticksLeft > 0) {
          --burst.ticksLeft;
        }
        const auto* const identity =
            components.get<game::NetworkIdentity>(projectile);
        if (burst.ticksLeft > 0 || identity == nullptr) {
          return;
        }
        bursts_.push_back(PendingBurst{.projectile = projectile,
                                       .networkId = identity->networkId,
                                       .center = position,
                                       .shardNumber = burst.shardNumber,
                                       .count = burst.count});
      });
}

void WeaponSystem::burst(const PendingBurst& pending) {
  world_->components().destroy(pending.projectile);
  events_->publish(
      game::EntityDestruction{.networkId = pending.networkId,
                              .cause = game::DestructionCause::Removed});
  for (std::size_t index = 0; index < pending.count; ++index) {
    const std::optional<network::EntitySpawned> shard =
        spawnShard(pending, index);
    if (!shard.has_value()) {
      return;
    }
    announce(*shard);
  }
}

std::optional<network::EntitySpawned> WeaponSystem::spawnShard(
    const PendingBurst& pending, std::size_t index) {
  const game::EntitySheet* const sheet = types_->numbered(pending.shardNumber);
  if (sheet == nullptr) {
    return std::nullopt;
  }
  const game::BodyPlacement placement = shardPlacementOf(pending, index);
  try {
    return entitySpawnedOf(world_->spawnFrom(*sheet, placement),
                           game::EntityTypeNumber{.number = sheet->number},
                           placement.position);
  } catch (const engine::NetworkIdExhaustedException&) {
    logger_.warn("no shard appeared: every network id was given");
    return std::nullopt;
  }
}

}  // namespace rtype::server

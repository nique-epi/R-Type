#include "WeaponSystem.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include "BehaviourConstants.hpp"
#include "BodyPlacement.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "CountsDownToReady.hpp"
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
#include "MessageDelivery.hpp"
#include "MessagePayload.hpp"
#include "Muzzle.hpp"
#include "NetworkIdentity.hpp"
#include "PendingBurst.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "ProjectileStartPosition.hpp"
#include "ProtocolConstants.hpp"
#include "ShardBurst.hpp"
#include "WeaponShot.hpp"
#include "WeaponSystemConstants.hpp"
#include "WeaponSystemParts.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/** @brief A projectile about to appear: its sheet, where it starts, and, for
 * a missile of a player, the player it belongs to and the inputNumber of the
 * newest input applied to that player's ship. */
struct ProjectileLaunch {
  std::reference_wrapper<const game::EntitySheet> sheet;
  game::Position start{};
  std::optional<std::uint8_t> owner;
  std::uint32_t firingInput{network::NO_FIRING_INPUT};
};

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
  ProjectileLaunch launch{
      .sheet = *sheet,
      .start = game::projectileStartPosition(shot.muzzle, sheet->hitbox),
      .owner = std::nullopt,
      .firingInput = network::NO_FIRING_INPUT};
  if (sheet->side == game::EntityType::PlayerMissile) {
    const ConnectedPlayer* const shooter = roster.findByShip(shot.shooterId);
    if (shooter == nullptr) {
      return std::nullopt;
    }
    launch.owner = shooter->playerNumber;
    launch.firingInput =
        shooter->appliedInputNumber.value_or(network::NO_FIRING_INPUT);
  }
  return launch;
}

/** @returns Where the shard @p index of @p pending appears and steers from:
 * SHARD_START_DISTANCE from the center of the projectile, along the direction
 * of that shard, y growing downwards. */
game::BodyPlacement shardPlacementOf(const PendingBurst& pending,
                                     std::size_t index) {
  const float angle = FIRST_SHARD_ANGLE_RADIANS +
                      (game::FULL_TURN_RADIANS * static_cast<float>(index) /
                       static_cast<float>(pending.burst.count));
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
        if (!game::countsDownToReady(gun) || !intent.shooting) {
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
  std::optional<network::EntitySpawned> spawned = spawnAt(
      launch->sheet.get(),
      game::BodyPlacement{.position = launch->start, .origin = launch->start},
      launch->owner);
  if (spawned.has_value()) {
    spawned->owner = launch->owner.value_or(network::NO_OWNER);
    spawned->firingInput = launch->firingInput;
  }
  return spawned;
}

std::optional<network::EntitySpawned> WeaponSystem::spawnAt(
    const game::EntitySheet& sheet, game::BodyPlacement placement,
    std::optional<std::uint8_t> owner) {
  try {
    return entitySpawnedOf(world_->spawnFrom(sheet, placement, owner),
                           game::EntityTypeNumber{.number = sheet.number},
                           placement.position);
  } catch (const engine::NetworkIdExhaustedException&) {
    logger_.warn("no projectile fired: every network id was given");
    return std::nullopt;
  }
}

void WeaponSystem::announce(const network::EntitySpawned& projectile) {
  if (projectile.owner != network::NO_OWNER) {
    guaranteeToEveryPlayer(*roster_, send_,
                           network::encodedMessage(projectile));
    return;
  }
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
        if (burst.ticksLeft > 0) {
          return;
        }
        const auto* const identity =
            components.get<game::NetworkIdentity>(projectile);
        if (identity == nullptr) {
          return;
        }
        bursts_.push_back(PendingBurst{.projectile = projectile,
                                       .networkId = identity->networkId,
                                       .center = position,
                                       .burst = burst});
      });
}

void WeaponSystem::burst(const PendingBurst& pending) {
  world_->components().destroy(pending.projectile);
  events_->publish(
      game::EntityDestruction{.networkId = pending.networkId,
                              .cause = game::DestructionCause::Removed});
  for (std::size_t index = 0; index < pending.burst.count; ++index) {
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
  const game::EntitySheet* const sheet =
      types_->numbered(pending.burst.shardNumber);
  if (sheet == nullptr) {
    return std::nullopt;
  }
  return spawnAt(*sheet, shardPlacementOf(pending, index), std::nullopt);
}

}  // namespace rtype::server

#include "WeaponSystem.hpp"
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "Gun.hpp"
#include "Intent.hpp"
#include "Muzzle.hpp"
#include "NetworkIdentity.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "ProjectileStartPosition.hpp"
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
  return network::EntitySpawned{.entityId = networkId,
                                .entityType = launch.sheet.get().number,
                                .x = launch.start.x,
                                .y = launch.start.y};
}

}  // namespace

WeaponSystem::WeaponSystem(WeaponSystemParts parts)
    : world_(&parts.world.get()),
      roster_(&parts.roster.get()),
      send_(std::move(parts.send)),
      types_(&parts.types.get()),
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

}  // namespace rtype::server

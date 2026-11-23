#include "OwnMissiles.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "AnimatedSprite.hpp"
#include "BodyMasks.hpp"
#include "BodyPlacement.hpp"
#include "Bounds.hpp"
#include "BoundsOf.hpp"
#include "CollidingBoxes.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "ContactShape.hpp"
#include "Direction.hpp"
#include "EffectSpawner.hpp"
#include "Entity.hpp"
#include "EntityCues.hpp"
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "EntitySprites.hpp"
#include "EntityType.hpp"
#include "EntityTypeConstants.hpp"
#include "EntityTypeLook.hpp"
#include "HeldWeapon.hpp"
#include "HitboxParts.hpp"
#include "IEntityTypeSource.hpp"
#include "Muzzle.hpp"
#include "OwnMissile.hpp"
#include "OwnMissilesConstants.hpp"
#include "OwnMissilesParts.hpp"
#include "PlacedMask.hpp"
#include "Position.hpp"
#include "PredictedCommand.hpp"
#include "ProjectileStartPosition.hpp"
#include "ShapeContacts.hpp"
#include "ShipArsenal.hpp"
#include "ShipGun.hpp"
#include "ShipGunReading.hpp"
#include "ShotPlacement.hpp"
#include "ShotStart.hpp"
#include "Sprite.hpp"
#include "TimeConstants.hpp"
#include "UnitHeading.hpp"

namespace rtype::client {

namespace {

/** @brief Where a predicted missile of a shot comes from: the input that
 * fired it, the center of the ship that fired it, and its rank in the shot. */
struct OwnShotStart {
  std::uint32_t firingInput{};
  game::Position ship{};
  std::uint8_t rank{0};
};

/** @returns The gun of the weapon @p held of the ships of @p types. */
const game::ShipGun& gunHeld(const IEntityTypeSource& types,
                             game::HeldWeapon held) {
  return game::heldGunOf(types.shipArsenal(), held);
}

/** @returns How fast missile @p rank of a shot of @p gun flies on each axis,
 * in logical units per second: the velocity of the gun for a missile that
 * flies straight, its scatterSpeed along the heading the shot gives that rank
 * for one that scatters (see game::ScatterBehaviour). */
game::Position velocityOf(const game::ShipGun& gun, std::uint8_t rank) {
  if (gun.scatterSpeed <= 0.0F) {
    return game::Position{.x = gun.velocityX, .y = gun.velocityY};
  }
  const game::BodyPlacement laid = game::shotPlacement(
      gun.pattern,
      game::ShotStart{.point = {}, .side = game::EntityType::Player}, rank);
  const game::Direction heading =
      game::headingAlong(game::Direction{.x = laid.position.x - laid.origin.x,
                                         .y = laid.position.y - laid.origin.y});
  return game::Position{.x = heading.x * gun.scatterSpeed,
                        .y = heading.y * gun.scatterSpeed};
}

/** @returns Missile @p start.rank of a shot of @p gun from a ship at
 * @p start.ship, laid as the server lays it (projectileStartPosition(), then
 * game::shotPlacement()); nothing when the session gives no box for the ship
 * or the missile. */
std::optional<OwnMissile> predictedMissileOf(const IEntityTypeSource& types,
                                             const game::ShipGun& gun,
                                             OwnShotStart start) {
  const std::optional<EntityTypeLook> ship = types.lookOf(gun.shipType);
  const std::optional<EntityTypeLook> missile =
      types.lookOf(gun.projectileType);
  if (!ship.has_value() || !missile.has_value()) {
    return std::nullopt;
  }
  const game::Position muzzle = game::projectileStartPosition(
      game::Muzzle{.center = start.ship,
                   .box = ship->hitbox,
                   .side = game::EntityType::Player},
      missile->hitbox);
  const game::BodyPlacement laid = game::shotPlacement(
      gun.pattern,
      game::ShotStart{.point = muzzle, .side = game::EntityType::Player},
      start.rank);
  return OwnMissile{.firingInput = start.firingInput,
                    .shotRank = start.rank,
                    .start = laid.position,
                    .velocity = velocityOf(gun, start.rank),
                    .projectileType = gun.projectileType,
                    .entity = std::nullopt,
                    .serverId = std::nullopt};
}

/** @returns The missile @p missile of the server stands for, flying as the
 * missile of its rank of a shot of @p gun. */
OwnMissile serverShotOf(const network::EntitySpawned& missile,
                        const game::ShipGun& gun) {
  return OwnMissile{.firingInput = missile.firingInput,
                    .shotRank = missile.shotRank,
                    .start = {.x = missile.x, .y = missile.y},
                    .velocity = velocityOf(gun, missile.shotRank),
                    .projectileType = missile.entityType,
                    .entity = std::nullopt,
                    .serverId = missile.entityId};
}

/** @returns Where a missile that left @p start flies after @p ticks ticks at
 * @p velocity. */
game::Position flownFrom(game::Position start, game::Position velocity,
                         std::uint32_t ticks) {
  const float seconds =
      std::chrono::duration<float>(engine::SIMULATION_TICK_DURATION).count() *
      static_cast<float>(ticks);
  return game::Position{.x = start.x + (velocity.x * seconds),
                        .y = start.y + (velocity.y * seconds)};
}

/** @brief Destroys the entity that draws @p missile, when it has one. */
void takeAway(engine::ComponentRegistry& components, OwnMissile& missile) {
  if (missile.entity.has_value()) {
    components.destroy(*missile.entity);
    missile.entity.reset();
  }
}

/** @returns The entity of @p missile at its start, looking as @p look, its
 * animation started from its firing input. */
engine::Entity drawnMissile(const OwnMissilesParts& parts,
                            const EntityTypeLook& look,
                            const OwnMissile& missile) {
  engine::ComponentRegistry& components = parts.components.get();
  const engine::Entity entity = parts.entities.get().create();
  components.add(entity, missile.start);
  components.add(entity, game::EntityType::PlayerMissile);
  components.add(entity, look.hitbox);
  AnimatedSprite sprite =
      animatedSpriteOf(look.appearance.get(), game::EntityType::PlayerMissile,
                       missile.firingInput);
  components.add(entity, std::move(sprite.sprite));
  components.add(entity, sprite.animation);
  return entity;
}

/** @returns How many inputs lie between @p first and @p second. */
std::uint32_t inputsBetween(std::uint32_t first, std::uint32_t second) {
  return first > second ? first - second : second - first;
}

/** @returns Whether @p missile was fired near @p firingInput: less than
 * @p intervalTicks divided by SHOT_MATCH_DIVISOR inputs from it. */
bool firedNear(const OwnMissile& missile, std::uint32_t firingInput,
               std::uint32_t intervalTicks) {
  return inputsBetween(missile.firingInput, firingInput) * SHOT_MATCH_DIVISOR <
         intervalTicks;
}

/** @returns Whether @p missile is the missile of the rank of @p shot, of its
 * type, fired near its input for the gun @p gun (see firedNear()). */
bool standsNear(const OwnMissile& missile, const OwnMissile& shot,
                const game::ShipGun& gun) {
  return missile.shotRank == shot.shotRank &&
         missile.projectileType == shot.projectileType &&
         firedNear(missile, shot.firingInput, gun.intervalTicks);
}

/** @returns The missile of @p missiles that no missile of the server stands
 * for yet fired nearest the input of @p shot, among those that stand near it
 * (see standsNear()); nullptr when there is none. */
OwnMissile* nearestUnmatched(std::vector<OwnMissile>& missiles,
                             const OwnMissile& shot, const game::ShipGun& gun) {
  OwnMissile* nearest = nullptr;
  for (OwnMissile& missile : missiles) {
    if (!missile.serverId.has_value() && standsNear(missile, shot, gun) &&
        (nearest == nullptr ||
         inputsBetween(missile.firingInput, shot.firingInput) <
             inputsBetween(nearest->firingInput, shot.firingInput))) {
      nearest = &missile;
    }
  }
  return nearest;
}

/** @returns Whether a missile of @p missiles that the server stands for
 * stands near @p shot (see standsNear()). */
bool serverFiredNear(const std::vector<OwnMissile>& missiles,
                     const OwnMissile& shot, const game::ShipGun& gun) {
  return std::ranges::any_of(
      missiles, [&shot, &gun](const OwnMissile& missile) {
        return missile.serverId.has_value() && standsNear(missile, shot, gun);
      });
}

/** @returns The missile @p shot, added to @p missiles and drawn with its shot
 * and its muzzle flash, looking as its type; nullptr, and nothing added, when
 * @p parts gives no look for it. */
OwnMissile* fireMissile(const OwnMissilesParts& parts,
                        std::vector<OwnMissile>& missiles, OwnMissile shot) {
  const std::optional<EntityTypeLook> look =
      parts.types.get().lookOf(shot.projectileType);
  if (!look.has_value()) {
    return nullptr;
  }
  shot.entity = drawnMissile(parts, *look, shot);
  missiles.push_back(shot);
  announce(parts.feedback, parts.components.get(), *shot.entity);
  return &missiles.back();
}

/** @brief Takes away and forgets every missile of @p missiles that
 * @p chosen chooses. */
template <typename Predicate>
void takeAwayIf(std::vector<OwnMissile>& missiles,
                engine::ComponentRegistry& components, Predicate chosen) {
  std::erase_if(missiles, [&components, &chosen](OwnMissile& missile) {
    if (!chosen(missile)) {
      return false;
    }
    takeAway(components, missile);
    return true;
  });
}

/** @brief Fills @p shapes with each box (game::collidingBoxesOf()) of every
 * enemy drawn on screen, with its collision mask laid where it is drawn
 * (game::bodyMaskAt()), as the server tests a missile against it. */
void collectEnemyShapes(engine::ComponentRegistry& components,
                        std::vector<game::ContactShape>& shapes) {
  shapes.clear();
  components.forEach<game::EntityType, game::Position>(
      [&components, &shapes](engine::Entity entity,
                             const game::EntityType& side,
                             const game::Position& position) {
        const auto* const box = components.get<game::CollisionBox>(entity);
        if (side != game::EntityType::Enemy || box == nullptr ||
            !components.has<game::Sprite>(entity)) {
          return;
        }
        const std::optional<engine::PlacedMask> mask =
            game::bodyMaskAt(components, entity, position);
        for (const game::CollisionBox& part : game::collidingBoxesOf(
                 *box, components.get<game::HitboxParts>(entity))) {
          shapes.push_back(game::ContactShape{
              .bounds = game::boundsOf(position, part), .mask = mask});
        }
      });
}

/** @returns The box of @p entity where it is drawn; nothing without one. */
std::optional<engine::Bounds> boundsOf(engine::ComponentRegistry& components,
                                       engine::Entity entity) {
  const auto* const position = components.get<game::Position>(entity);
  const auto* const box = components.get<game::CollisionBox>(entity);
  if (position == nullptr || box == nullptr) {
    return std::nullopt;
  }
  return game::boundsOf(*position, *box);
}

/** @returns Whether the box of @p missile touches one of @p enemies: its
 * box, then its mask when it has one (game::shapesTouch()). The missile counts
 * as its box, as the server tests it. */
bool touchesAnEnemy(const engine::Bounds& missile,
                    const std::vector<game::ContactShape>& enemies) {
  return std::ranges::any_of(
      enemies, [&missile](const game::ContactShape& enemy) {
        return game::shapesTouch(game::ContactShape{.bounds = missile}, enemy);
      });
}

}  // namespace

OwnMissiles::OwnMissiles(OwnMissilesParts parts) : parts_(parts) {}

void OwnMissiles::restart(std::uint32_t acknowledged, game::Position ship) {
  held_ = game::HeldWeapon{};
  for (const PredictedCommand& shot :
       gun_.arm(parts_.types.get().shipArsenal().gun, acknowledged)) {
    fireOrMove(shot.inputNumber, ship);
  }
  place();
}

void OwnMissiles::follow(std::span<const PredictedCommand> commands,
                         bool mayFire) {
  for (const PredictedCommand& command : commands) {
    newestInput_ = command.inputNumber;
    if (!mayFire) {
      gun_.record(command);
    } else if (gun_.fires(command) && active_) {
      fireOrMove(command.inputNumber, command.shipAfter);
    }
  }
  place();
}

void OwnMissiles::acknowledge(std::uint32_t acknowledged,
                              game::HeldWeapon held) {
  if (held != held_) {
    held_ = held;
    gun_.hold(gunHeld(parts_.types.get(), held_));
  }
  if (acknowledged < REFUSED_SHOT_TICKS) {
    return;
  }
  const std::uint32_t lastRefusable =
      acknowledged - static_cast<std::uint32_t>(REFUSED_SHOT_TICKS);
  takeAwayIf(missiles_, parts_.components.get(),
             [lastRefusable](const OwnMissile& missile) {
               return !missile.serverId.has_value() &&
                      missile.firingInput <= lastRefusable;
             });
  gun_.forgetUpTo(lastRefusable);
}

void OwnMissiles::touchEnemies() {
  engine::ComponentRegistry& components = parts_.components.get();
  if (std::ranges::none_of(missiles_, [](const OwnMissile& missile) {
        return missile.entity.has_value();
      })) {
    return;
  }
  collectEnemyShapes(components, enemyShapes_);
  for (OwnMissile& missile : missiles_) {
    if (!missile.entity.has_value()) {
      continue;
    }
    const std::optional<engine::Bounds> bounds =
        boundsOf(components, *missile.entity);
    if (bounds.has_value() && touchesAnEnemy(*bounds, enemyShapes_)) {
      parts_.feedback.effects.get().playExplosion(
          game::EntityType::PlayerMissile,
          game::Position{.x = bounds->centerX, .y = bounds->centerY});
      takeAway(components, missile);
    }
  }
}

void OwnMissiles::activate() { active_ = true; }

bool OwnMissiles::predictsOwnMissiles() const {
  return active_ && gunHeld(parts_.types.get(), held_).projectileType !=
                        identifiers::NO_ENTITY_TYPE;
}

void OwnMissiles::ownMissileSpawned(const network::EntitySpawned& missile) {
  const game::ShipGun& gun = gunHeld(parts_.types.get(), held_);
  const OwnMissile serverShot = serverShotOf(missile, gun);
  OwnMissile* matched = nearestUnmatched(missiles_, serverShot, gun);
  if (matched == nullptr) {
    matched = fireMissile(parts_, missiles_, serverShot);
  }
  if (matched != nullptr) {
    matched->firingInput = serverShot.firingInput;
    matched->start = serverShot.start;
    matched->velocity = serverShot.velocity;
    matched->serverId = serverShot.serverId;
  }
  const std::vector<PredictedCommand> firing =
      gun_.confirm(missile.firingInput);
  for (const PredictedCommand& shot : firing) {
    fireOrMove(shot.inputNumber, shot.shipAfter);
  }
  takeAwayIf(missiles_, parts_.components.get(),
             [&missile, &firing](const OwnMissile& shot) {
               return !shot.serverId.has_value() &&
                      shot.firingInput > missile.firingInput &&
                      std::ranges::none_of(
                          firing, [&shot](const PredictedCommand& fired) {
                            return fired.inputNumber == shot.firingInput;
                          });
             });
  place();
}

void OwnMissiles::ownMissileDestroyed(
    const network::EntityDestroyed& destroyed) {
  takeAwayIf(missiles_, parts_.components.get(),
             [&destroyed](const OwnMissile& missile) {
               return missile.serverId == destroyed.entityId;
             });
}

void OwnMissiles::fireOrMove(std::uint32_t firingInput, game::Position ship) {
  const game::ShipGun& gun = gunHeld(parts_.types.get(), held_);
  for (std::uint8_t rank = 0; rank < gun.pattern.count; ++rank) {
    const std::optional<OwnMissile> shot = predictedMissileOf(
        parts_.types.get(), gun,
        OwnShotStart{.firingInput = firingInput, .ship = ship, .rank = rank});
    if (!shot.has_value() || serverFiredNear(missiles_, *shot, gun)) {
      continue;
    }
    if (OwnMissile* const near = nearestUnmatched(missiles_, *shot, gun)) {
      near->firingInput = shot->firingInput;
      near->start = shot->start;
      near->velocity = shot->velocity;
      continue;
    }
    fireMissile(parts_, missiles_, *shot);
  }
}

void OwnMissiles::place() {
  for (const OwnMissile& missile : missiles_) {
    if (!missile.entity.has_value() || newestInput_ < missile.firingInput) {
      continue;
    }
    if (auto* const position =
            parts_.components.get().get<game::Position>(*missile.entity)) {
      *position = flownFrom(missile.start, missile.velocity,
                            newestInput_ - missile.firingInput);
    }
  }
}

}  // namespace rtype::client

#include "OwnMissiles.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "AnimatedSprite.hpp"
#include "Bounds.hpp"
#include "BoundsOf.hpp"
#include "CollidingBoxes.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "Entity.hpp"
#include "EntityCues.hpp"
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "EntitySprites.hpp"
#include "EntityType.hpp"
#include "EntityTypeConstants.hpp"
#include "EntityTypeLook.hpp"
#include "HitboxParts.hpp"
#include "IEntityTypeSource.hpp"
#include "Muzzle.hpp"
#include "Overlaps.hpp"
#include "OwnMissile.hpp"
#include "OwnMissilesConstants.hpp"
#include "OwnMissilesParts.hpp"
#include "Position.hpp"
#include "PredictedCommand.hpp"
#include "ProjectileStartPosition.hpp"
#include "ShipGun.hpp"
#include "Sprite.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Where a missile of the gun of @p types leaves a ship at
 * @p shipCenter, as the server puts it (projectileStartPosition()); nothing
 * when the session gives no box for the ship or the missile. */
std::optional<game::Position> startOf(const IEntityTypeSource& types,
                                      game::Position shipCenter) {
  const game::ShipGun gun = types.shipGun();
  const std::optional<EntityTypeLook> ship = types.lookOf(gun.shipType);
  const std::optional<EntityTypeLook> missile =
      types.lookOf(gun.projectileType);
  if (!ship.has_value() || !missile.has_value()) {
    return std::nullopt;
  }
  return game::projectileStartPosition(
      game::Muzzle{.center = shipCenter,
                   .box = ship->hitbox,
                   .side = game::EntityType::Player},
      missile->hitbox);
}

/** @returns Where a missile that left @p start flies after @p ticks ticks at
 * the velocity of @p gun. */
game::Position flownFrom(game::Position start, const game::ShipGun& gun,
                         std::uint32_t ticks) {
  const float seconds =
      std::chrono::duration<float>(engine::SIMULATION_TICK_DURATION).count() *
      static_cast<float>(ticks);
  return game::Position{.x = start.x + (gun.velocityX * seconds),
                        .y = start.y + (gun.velocityY * seconds)};
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

/** @returns The missile of @p missiles that no missile of the server stands
 * for yet fired nearest @p firingInput, among those fired near it (see
 * firedNear()) for the gun @p gun; nullptr when there is none. */
OwnMissile* nearestUnmatched(std::vector<OwnMissile>& missiles,
                             std::uint32_t firingInput,
                             const game::ShipGun& gun) {
  OwnMissile* nearest = nullptr;
  for (OwnMissile& missile : missiles) {
    if (!missile.serverId.has_value() &&
        firedNear(missile, firingInput, gun.intervalTicks) &&
        (nearest == nullptr ||
         inputsBetween(missile.firingInput, firingInput) <
             inputsBetween(nearest->firingInput, firingInput))) {
      nearest = &missile;
    }
  }
  return nearest;
}

/** @returns Whether a missile of @p missiles that the server stands for was
 * fired near @p firingInput for the gun @p gun. */
bool serverFiredNear(const std::vector<OwnMissile>& missiles,
                     std::uint32_t firingInput, const game::ShipGun& gun) {
  return std::ranges::any_of(
      missiles, [firingInput, &gun](const OwnMissile& missile) {
        return missile.serverId.has_value() &&
               firedNear(missile, firingInput, gun.intervalTicks);
      });
}

/** @returns The missile @p shot, added to @p missiles and drawn with its shot
 * and its muzzle flash; nullptr, and nothing added, when @p parts gives no
 * look for it. */
OwnMissile* fireMissile(const OwnMissilesParts& parts,
                        std::vector<OwnMissile>& missiles, OwnMissile shot) {
  const std::optional<EntityTypeLook> look =
      parts.types.get().lookOf(parts.types.get().shipGun().projectileType);
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

/** @brief Fills @p boxes with each box (game::collidingBoxesOf()) of every
 * enemy drawn on screen. */
void collectEnemyBoxes(engine::ComponentRegistry& components,
                       std::vector<engine::Bounds>& boxes) {
  boxes.clear();
  components.forEach<game::EntityType, game::Position>(
      [&components, &boxes](engine::Entity entity, const game::EntityType& side,
                            const game::Position& position) {
        const auto* const box = components.get<game::CollisionBox>(entity);
        if (side != game::EntityType::Enemy || box == nullptr ||
            !components.has<game::Sprite>(entity)) {
          return;
        }
        for (const game::CollisionBox& part : game::collidingBoxesOf(
                 *box, components.get<game::HitboxParts>(entity))) {
          boxes.push_back(game::boundsOf(position, part));
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

/** @returns Whether @p missile overlaps one of @p enemies. */
bool touchesAnEnemy(const engine::Bounds& missile,
                    const std::vector<engine::Bounds>& enemies) {
  return std::ranges::any_of(enemies, [&missile](const engine::Bounds& enemy) {
    return engine::overlaps(missile, enemy);
  });
}

}  // namespace

OwnMissiles::OwnMissiles(OwnMissilesParts parts) : parts_(parts) {}

void OwnMissiles::restart(std::uint32_t acknowledged, game::Position ship) {
  for (const PredictedCommand& shot :
       gun_.arm(parts_.types.get().shipGun(), acknowledged)) {
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

void OwnMissiles::acknowledge(std::uint32_t acknowledged) {
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
  collectEnemyBoxes(components, enemyBoxes_);
  for (OwnMissile& missile : missiles_) {
    if (!missile.entity.has_value()) {
      continue;
    }
    const std::optional<engine::Bounds> bounds =
        boundsOf(components, *missile.entity);
    if (bounds.has_value() && touchesAnEnemy(*bounds, enemyBoxes_)) {
      parts_.feedback.effects.get().playExplosion(
          game::EntityType::PlayerMissile,
          game::Position{.x = bounds->centerX, .y = bounds->centerY});
      takeAway(components, missile);
    }
  }
}

void OwnMissiles::activate() { active_ = true; }

bool OwnMissiles::predictsOwnMissiles() const {
  return active_ && parts_.types.get().shipGun().projectileType !=
                        identifiers::NO_ENTITY_TYPE;
}

void OwnMissiles::ownMissileSpawned(const network::EntitySpawned& missile) {
  const game::ShipGun gun = parts_.types.get().shipGun();
  const OwnMissile serverShot{.firingInput = missile.firingInput,
                              .start = {.x = missile.x, .y = missile.y},
                              .entity = std::nullopt,
                              .serverId = missile.entityId};
  OwnMissile* matched = nearestUnmatched(missiles_, missile.firingInput, gun);
  if (matched == nullptr) {
    matched = fireMissile(parts_, missiles_, serverShot);
  }
  if (matched != nullptr) {
    matched->firingInput = serverShot.firingInput;
    matched->start = serverShot.start;
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
  const game::ShipGun gun = parts_.types.get().shipGun();
  const std::optional<game::Position> start = startOf(parts_.types.get(), ship);
  if (!start.has_value() || serverFiredNear(missiles_, firingInput, gun)) {
    return;
  }
  if (OwnMissile* const near = nearestUnmatched(missiles_, firingInput, gun)) {
    near->firingInput = firingInput;
    near->start = *start;
    return;
  }
  fireMissile(parts_, missiles_,
              OwnMissile{.firingInput = firingInput,
                         .start = *start,
                         .entity = std::nullopt,
                         .serverId = std::nullopt});
}

void OwnMissiles::place() {
  const game::ShipGun gun = parts_.types.get().shipGun();
  for (const OwnMissile& missile : missiles_) {
    if (!missile.entity.has_value() || newestInput_ < missile.firingInput) {
      continue;
    }
    if (auto* const position =
            parts_.components.get().get<game::Position>(*missile.entity)) {
      *position =
          flownFrom(missile.start, gun, newestInput_ - missile.firingInput);
    }
  }
}

}  // namespace rtype::client

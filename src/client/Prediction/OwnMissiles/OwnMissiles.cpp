#include "OwnMissiles.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>
#include "AnimatedSprite.hpp"
#include "Bounds.hpp"
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
  const network::ShipGun gun = types.shipGun();
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
game::Position flownFrom(game::Position start, const network::ShipGun& gun,
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

/** @returns The missile of @p missiles fired by @p firingInput that no
 * missile of the server stands for yet; nullptr when there is none. */
OwnMissile* unmatchedOf(std::vector<OwnMissile>& missiles,
                        std::uint32_t firingInput) {
  const auto found =
      std::ranges::find_if(missiles, [firingInput](const OwnMissile& missile) {
        return !missile.serverId.has_value() &&
               missile.firingInput == firingInput;
      });
  return found == missiles.end() ? nullptr : &*found;
}

/** @brief Takes away and forgets every missile of @p missiles that
 * @p chosen chooses. */
template <typename Predicate>
void takeAwayIf(std::vector<OwnMissile>& missiles,
                engine::ComponentRegistry& components, Predicate chosen) {
  for (OwnMissile& missile : missiles) {
    if (chosen(missile)) {
      takeAway(components, missile);
    }
  }
  std::erase_if(missiles, chosen);
}

/** @returns Whether the box @p missile overlaps an enemy drawn on screen. */
bool touchesAnEnemy(engine::ComponentRegistry& components,
                    const engine::Bounds& missile) {
  bool touches = false;
  components.forEach<game::EntityType, game::Position>(
      [&components, &missile, &touches](engine::Entity entity,
                                        const game::EntityType& side,
                                        const game::Position& position) {
        const auto* const box = components.get<game::CollisionBox>(entity);
        if (side != game::EntityType::Enemy || box == nullptr ||
            !components.has<game::Sprite>(entity)) {
          return;
        }
        touches =
            touches ||
            engine::overlaps(missile, engine::Bounds{.centerX = position.x,
                                                     .centerY = position.y,
                                                     .width = box->width,
                                                     .height = box->height});
      });
  return touches;
}

/** @returns The box of @p entity where it is drawn; nothing without one. */
std::optional<engine::Bounds> boundsOf(engine::ComponentRegistry& components,
                                       engine::Entity entity) {
  const auto* const position = components.get<game::Position>(entity);
  const auto* const box = components.get<game::CollisionBox>(entity);
  if (position == nullptr || box == nullptr) {
    return std::nullopt;
  }
  return engine::Bounds{.centerX = position->x,
                        .centerY = position->y,
                        .width = box->width,
                        .height = box->height};
}

}  // namespace

OwnMissiles::OwnMissiles(OwnMissilesParts parts) : parts_(parts) {}

void OwnMissiles::restart() { gun_.arm(parts_.types.get().shipGun()); }

void OwnMissiles::follow(std::span<const PredictedCommand> commands) {
  for (const PredictedCommand& command : commands) {
    newestInput_ = command.inputNumber;
    if (gun_.fires(command) && active_) {
      if (const std::optional<game::Position> start =
              startOf(parts_.types.get(), command.shipAfter)) {
        fire(command.inputNumber, *start);
      }
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
  for (OwnMissile& missile : missiles_) {
    if (!missile.entity.has_value()) {
      continue;
    }
    const std::optional<engine::Bounds> bounds =
        boundsOf(components, *missile.entity);
    if (bounds.has_value() && touchesAnEnemy(components, *bounds)) {
      parts_.feedback.effects.get().playExplosion(
          game::EntityType::PlayerMissile,
          game::Position{.x = bounds->centerX, .y = bounds->centerY});
      takeAway(components, missile);
    }
  }
}

void OwnMissiles::setActive(bool active) {
  if (!active) {
    for (OwnMissile& missile : missiles_) {
      takeAway(parts_.components.get(), missile);
    }
    missiles_.clear();
  }
  active_ = active;
}

bool OwnMissiles::predictsOwnMissiles() const {
  return active_ && parts_.types.get().shipGun().projectileType !=
                        identifiers::NO_ENTITY_TYPE;
}

void OwnMissiles::ownMissileSpawned(const network::EntitySpawned& missile) {
  const game::Position serverStart{.x = missile.x, .y = missile.y};
  if (unmatchedOf(missiles_, missile.firingInput) == nullptr) {
    fire(missile.firingInput, serverStart);
  }
  if (OwnMissile* const matched = unmatchedOf(missiles_, missile.firingInput)) {
    matched->serverId = missile.entityId;
    matched->start = serverStart;
  }
  const std::vector<PredictedCommand> firing =
      gun_.confirm(missile.firingInput);
  takeAwayIf(missiles_, parts_.components.get(),
             [&missile, &firing](const OwnMissile& shot) {
               return !shot.serverId.has_value() &&
                      shot.firingInput > missile.firingInput &&
                      std::ranges::none_of(
                          firing, [&shot](const PredictedCommand& fired) {
                            return fired.inputNumber == shot.firingInput;
                          });
             });
  for (const PredictedCommand& shot : firing) {
    const std::optional<game::Position> start =
        startOf(parts_.types.get(), shot.shipAfter);
    if (unmatchedOf(missiles_, shot.inputNumber) == nullptr &&
        start.has_value()) {
      fire(shot.inputNumber, *start);
    }
  }
  place();
}

void OwnMissiles::ownMissileDestroyed(
    const network::EntityDestroyed& destroyed) {
  const auto found =
      std::ranges::find_if(missiles_, [&destroyed](const OwnMissile& missile) {
        return missile.serverId == destroyed.entityId;
      });
  if (found == missiles_.end()) {
    return;
  }
  takeAway(parts_.components.get(), *found);
  missiles_.erase(found);
}

void OwnMissiles::fire(std::uint32_t firingInput, game::Position start) {
  const std::optional<EntityTypeLook> look =
      parts_.types.get().lookOf(parts_.types.get().shipGun().projectileType);
  if (!look.has_value()) {
    return;
  }
  OwnMissile missile{.firingInput = firingInput,
                     .start = start,
                     .entity = std::nullopt,
                     .serverId = std::nullopt};
  missile.entity = drawnMissile(parts_, *look, missile);
  missiles_.push_back(missile);
  announce(parts_.feedback, parts_.components.get(), *missile.entity);
}

void OwnMissiles::place() {
  const network::ShipGun gun = parts_.types.get().shipGun();
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

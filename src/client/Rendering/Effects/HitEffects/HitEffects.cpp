#include "HitEffects.hpp"
#include <optional>
#include "ComponentRegistry.hpp"
#include "EffectColorConstants.hpp"
#include "EffectConstants.hpp"
#include "EffectTargets.hpp"
#include "EffectTierConstants.hpp"
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "FlashGate.hpp"
#include "GroundEffectConstants.hpp"
#include "HitSpot.hpp"
#include "Nudge.hpp"
#include "PixelEffects.hpp"
#include "Position.hpp"
#include "SheetEffect.hpp"
#include "SheetEffects.hpp"
#include "Sprite.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The direction the shooter of a shot that touched an entity of
 * that type stands in: the players shoot to the right, the enemies to the
 * left. */
float towardsShooterOf(game::EntityType type) {
  switch (type) {
    case game::EntityType::Enemy:
    case game::EntityType::PlayerMissile:
      return TOWARDS_LEFT;
    case game::EntityType::Player:
    case game::EntityType::EnemyMissile:
      break;
  }
  return TOWARDS_RIGHT;
}

/** @returns The knockback of an entity of that type hit: drawn a pixel
 * along the shot, a ship to the left, anything else to the right, for
 * KNOCKBACK_DURATION. */
Nudge knockbackOf(game::EntityType type) {
  return Nudge{.amount = type == game::EntityType::Player ? -KNOCKBACK_OFFSET
                                                          : KNOCKBACK_OFFSET,
               .flipEvery = engine::Duration::zero(),
               .elapsed = engine::Duration::zero(),
               .duration = KNOCKBACK_DURATION};
}

/** @returns The pixels the silhouette of an entity covers: a ship's, or a
 * Bydo's, the largest, for anything else. */
int silhouetteAreaOf(const game::EntityType* type) {
  return type != nullptr && *type == game::EntityType::Player
             ? SHIP_SILHOUETTE_AREA
             : BYDO_SILHOUETTE_AREA;
}

}  // namespace

HitEffects::HitEffects(EffectTools tools) : tools_(tools) {}

void HitEffects::playHit(engine::Entity entity) {
  engine::ComponentRegistry& components = tools_.components;
  if (!components.has<game::Sprite>(entity)) {
    return;
  }
  flash(entity);
  if (const std::optional<HitSpot> spot = hitSpotOf(components, entity);
      spot.has_value()) {
    tools_.gate.get().nudge(components, entity, knockbackOf(spot->type));
    burstSparks(spot->type, spot->center);
  }
}

void HitEffects::playImpact(game::EntityType type, game::Position center) {
  spawnSheetEffect(tools_,
                   SheetEffect{.animations = &IMPACT,
                               .center = center,
                               .hold = engine::Duration::zero(),
                               .flash = {.area = IMPACT_FLASH_AREA,
                                         .remaining = IMPACT_FRAME_TIME *
                                                      IMPACT_FLASH_FRAMES}});
  burstSparks(type, center);
}

void HitEffects::flash(engine::Entity entity) const {
  engine::ComponentRegistry& components = tools_.components;
  const int area = silhouetteAreaOf(components.get<game::EntityType>(entity));
  components.add(entity, tools_.gate.get().hitFlashFor(
                             {.area = area, .remaining = HIT_FLASH_DURATION}));
}

void HitEffects::burstSparks(game::EntityType type,
                             game::Position center) const {
  tools_.pixels.get().burst({.shape = IMPACT_SPARKS,
                             .center = center,
                             .aim = towardsShooterOf(type),
                             .colors = tools_.gate.get().rampOf(SPARK_RAMP),
                             .delay = engine::Duration::zero()});
}

}  // namespace rtype::client

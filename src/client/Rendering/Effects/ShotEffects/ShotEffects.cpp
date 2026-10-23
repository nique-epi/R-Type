#include "ShotEffects.hpp"
#include <optional>
#include "Anchor.hpp"
#include "ComponentRegistry.hpp"
#include "EffectConstants.hpp"
#include "EffectTargets.hpp"
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "FlashGate.hpp"
#include "Nudge.hpp"
#include "Position.hpp"
#include "SheetEffect.hpp"
#include "SheetEffects.hpp"
#include "Shooter.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ShotEffects::ShotEffects(EffectTools tools) : tools_(tools) {}

void ShotEffects::playPlayerShot(game::Position center) {
  const std::optional<engine::Entity> muzzle = spawnSheetEffect(
      tools_, SheetEffect{.animations = &PLAYER_MUZZLE_FLASH,
                          .center = center,
                          .hold = engine::Duration::zero(),
                          .flash = {.area = MUZZLE_FLASH_AREA,
                                    .remaining = MUZZLE_FLASH_FRAME_TIME}});
  if (muzzle.has_value()) {
    followShooter(*muzzle, center);
  }
}

void ShotEffects::playEnemyShot(game::Position center) {
  spawnSheetEffect(
      tools_, SheetEffect{.animations = &ENEMY_MUZZLE_FLASH,
                          .center = center,
                          .hold = engine::Duration::zero(),
                          .flash = {.area = MUZZLE_FLASH_AREA,
                                    .remaining = MUZZLE_FLASH_FRAME_TIME}});
}

void ShotEffects::followShooter(engine::Entity muzzle,
                                game::Position center) const {
  engine::ComponentRegistry& components = tools_.components;
  const std::optional<Shooter> shooter = shooterOf(components, center);
  if (!shooter.has_value()) {
    return;
  }
  components.add(muzzle, Anchor{.entity = shooter->ship,
                                .offset = {center.x - shooter->center.x,
                                           center.y - shooter->center.y}});
  tools_.gate.get().nudge(components, shooter->ship,
                          Nudge{.amount = RECOIL_OFFSET,
                                .flipEvery = engine::Duration::zero(),
                                .elapsed = engine::Duration::zero(),
                                .duration = RECOIL_DURATION});
}

}  // namespace rtype::client

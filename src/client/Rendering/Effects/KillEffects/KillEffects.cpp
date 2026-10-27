#include "KillEffects.hpp"
#include <SFML/Graphics/Color.hpp>
#include <chrono>
#include <cstddef>
#include <optional>
#include <span>
#include "CameraShakeConstants.hpp"
#include "ComponentRegistry.hpp"
#include "DownedShip.hpp"
#include "EffectColorConstants.hpp"
#include "EffectConstants.hpp"
#include "EffectTargets.hpp"
#include "EffectTierConstants.hpp"
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "FlashGate.hpp"
#include "FlashLoan.hpp"
#include "GainFigures.hpp"
#include "GroundEffectConstants.hpp"
#include "KillTier.hpp"
#include "Layer.hpp"
#include "Lifetime.hpp"
#include "Nudge.hpp"
#include "PilotDebrisRamps.hpp"
#include "PilotShades.hpp"
#include "PixelEffects.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"
#include "SheetEffect.hpp"
#include "SheetEffects.hpp"
#include "Sprite.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The pixels a ship explosion flashes, from now until its first
 * SHIP_EXPLOSION_FLASH_FRAMES frames passed after its hold. */
FlashLoan shipExplosionFlash(engine::Duration hold) {
  return FlashLoan{.area = SHIP_EXPLOSION_FLASH_AREA,
                   .remaining = hold + (SHIP_EXPLOSION_FRAME_TIME *
                                        SHIP_EXPLOSION_FLASH_FRAMES)};
}

/** @returns The colors the debris of the ship of a pilot go through, among
 * the ramps of the palette the player chose. */
std::span<const sf::Color> debrisRampOf(const PilotDebrisRamps& ramps,
                                        std::optional<int> pilotColor) {
  if (!pilotColor.has_value()) {
    return UNKNOWN_PILOT_DEBRIS_RAMP;
  }
  return ramps[static_cast<std::size_t>(*pilotColor)];
}

/** @returns The colors the shockwave of the ship of a pilot goes through:
 * its debris ramp without the white. */
std::span<const sf::Color> ringRampOf(const PilotDebrisRamps& ramps,
                                      std::optional<int> pilotColor) {
  if (!pilotColor.has_value()) {
    return UNKNOWN_PILOT_RING_RAMP;
  }
  return debrisRampOf(ramps, pilotColor).subspan(1);
}

/** @returns The vibration of a victim frozen for @p hold: one pixel to each
 * side in turn, over the first half of the freeze. */
Nudge vibrationOf(engine::Duration hold) {
  return Nudge{
      .amount = VIBRATION_OFFSET,
      .flipEvery = VIBRATION_STEP,
      .elapsed = engine::Duration::zero(),
      .duration = std::chrono::duration_cast<engine::Duration>(hold * HALF)};
}

}  // namespace

KillEffects::KillEffects(EffectTools tools, GainFigures& gains)
    : tools_(tools), gains_(&gains) {}

void KillEffects::advance(engine::Duration elapsed) {
  killChain_.advance(elapsed);
}

void KillEffects::playKill(game::Position center) {
  const FlashGate& gate = tools_.gate;
  PixelEffects& pixels = tools_.pixels;
  const bool bigCatch = killChain_.noteKill();
  const KillTier& tier = bigCatch ? BIG_CATCH_TIER : KILL_TIER;
  const std::optional<engine::Entity> explosion = spawnSheetEffect(
      tools_, SheetEffect{.animations = &BYDO_EXPLOSION,
                          .center = center,
                          .hold = tier.hold,
                          .flash = {.area = BYDO_EXPLOSION_FLASH_AREA,
                                    .remaining = tier.hold +
                                                 BYDO_EXPLOSION_FRAME_TIME}});
  if (explosion.has_value()) {
    gate.nudge(tools_.components, *explosion, vibrationOf(tier.hold));
  }
  pixels.burst({.shape = tier.drops,
                .center = center,
                .aim = TOWARDS_RIGHT,
                .colors = gate.rampOf(BYDO_RAMP),
                .delay = tier.hold});
  if (tier.ring.has_value()) {
    pixels.wave({.shape = *tier.ring,
                 .center = center,
                 .colors = gate.rampOf(BIG_CATCH_RING_RAMP),
                 .delay = tier.hold});
  }
  gate.shake(tools_.camera, tier.trauma);
  gains_->noteKill(center, bigCatch);
}

void KillEffects::playShipExplosion(game::Position center) {
  spawnSheetEffect(
      tools_,
      SheetEffect{.animations = &SHIP_EXPLOSION,
                  .center = center,
                  .hold = engine::Duration::zero(),
                  .flash = shipExplosionFlash(engine::Duration::zero())});
}

void KillEffects::playShipDown(engine::Entity ship) {
  const std::optional<DownedShip> downed =
      downedShipOf(tools_.components, ship);
  if (!downed.has_value()) {
    return;
  }
  const FlashGate& gate = tools_.gate;
  PixelEffects& pixels = tools_.pixels;
  const PilotShades& shades = tools_.shades;
  freeze(*downed);
  spawnSheetEffect(tools_,
                   SheetEffect{.animations = &SHIP_EXPLOSION,
                               .center = downed->center,
                               .hold = SHIP_DOWN_HOLD,
                               .flash = shipExplosionFlash(SHIP_DOWN_HOLD)});
  pixels.burst(
      {.shape = SHIP_DEBRIS,
       .center = downed->center,
       .aim = TOWARDS_RIGHT,
       .colors = gate.rampOf(debrisRampOf(shades.debris, downed->pilotColor)),
       .delay = SHIP_DOWN_HOLD});
  pixels.wave({.shape = SHIP_DOWN_RING,
               .center = downed->center,
               .colors = ringRampOf(shades.debris, downed->pilotColor),
               .delay = SHIP_DOWN_HOLD});
  gate.shake(tools_.camera, SHIP_DOWN_TRAUMA);
}

void KillEffects::freeze(const DownedShip& ship) const {
  engine::ComponentRegistry& components = tools_.components;
  const engine::Entity picture = tools_.entities.get().create();
  components.add(picture, ship.center);
  components.add(picture, game::Sprite{.assetId = ship.sprite.assetId,
                                       .frame = ship.sprite.frame,
                                       .layer = game::Layer::Players});
  components.add(picture,
                 tools_.gate.get().hitFlashFor({.area = SHIP_SILHOUETTE_AREA,
                                                .remaining = SHIP_DOWN_HOLD}));
  components.add(picture, Lifetime{.remaining = SHIP_DOWN_HOLD});
  tools_.gate.get().nudge(components, picture, vibrationOf(SHIP_DOWN_HOLD));
}

}  // namespace rtype::client

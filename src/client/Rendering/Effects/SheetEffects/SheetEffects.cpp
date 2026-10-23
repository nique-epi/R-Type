#include "SheetEffects.hpp"
#include <optional>
#include "Animation.hpp"
#include "ComponentRegistry.hpp"
#include "EffectAnimations.hpp"
#include "EffectConstants.hpp"
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "FlashGate.hpp"
#include "Layer.hpp"
#include "SheetEffect.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The standard version of the effect when the gate lets it flash,
 * else the version without flash. */
const Animation& versionOf(FlashGate& gate, const SheetEffect& effect) {
  return gate.mayFlash(effect.flash) ? effect.animations->standard
                                     : effect.animations->withoutFlash;
}

/** @returns A new entity showing the first frame of the animation, which
 * starts at minus the hold of the effect. */
std::optional<engine::Entity> addSheetEntity(const EffectTools& tools,
                                             const Animation& animation,
                                             const SheetEffect& effect) {
  const std::optional<game::SpriteFrame> firstFrame =
      animation.frameAt(engine::Duration::zero());
  if (!firstFrame.has_value()) {
    return std::nullopt;
  }
  engine::ComponentRegistry& components = tools.components;
  const engine::Entity entity = tools.entities.get().create();
  components.add(entity, effect.center);
  components.add(entity, game::Sprite{.assetId = EFFECT_SHEET_ID,
                                      .frame = *firstFrame,
                                      .layer = game::Layer::Effects});
  components.add(
      entity, SpriteAnimation{.animation = animation, .elapsed = -effect.hold});
  return entity;
}

}  // namespace

std::optional<engine::Entity> spawnSheetEffect(const EffectTools& tools,
                                               const SheetEffect& effect) {
  return addSheetEntity(tools, versionOf(tools.gate, effect), effect);
}

}  // namespace rtype::client

#include "EffectPlayer.hpp"
#include <algorithm>
#include <optional>
#include <string>
#include "AppearanceReference.hpp"
#include "ComponentRegistry.hpp"
#include "EffectAnimations.hpp"
#include "EffectPlayed.hpp"
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "EntityWithAppearance.hpp"
#include "NamedEffect.hpp"
#include "Playback.hpp"
#include "Position.hpp"
#include "SheetEffect.hpp"
#include "SheetEffects.hpp"

namespace rtype::client {

namespace {

/** @returns The effect @p appearance names @p name, or nothing. */
const NamedEffect* effectNamed(const EntityAppearance& appearance,
                               const std::string& name) {
  const auto found =
      std::ranges::find(appearance.effects, name, &NamedEffect::name);
  return found == appearance.effects.end() ? nullptr : &*found;
}

/** @returns The two versions of @p effect, played once, hanging from the
 * center of the body by its anchor; they view its frames. */
EffectAnimations animationsOf(const NamedEffect& effect) {
  return EffectAnimations{
      .standard = {.frames = effect.frames,
                   .frameDuration = effect.frameDuration,
                   .playback = Playback::Once,
                   .anchor = effect.anchor},
      .withoutFlash = {.frames = effect.framesWithoutFlash,
                       .frameDuration = effect.frameDuration,
                       .playback = Playback::Once,
                       .anchor = effect.anchor}};
}

}  // namespace

EffectPlayer::EffectPlayer(EffectTools tools) : tools_(tools) {}

void EffectPlayer::apply(const network::EffectPlayed& played) {
  engine::ComponentRegistry& components = tools_.components;
  const std::optional<engine::Entity> body =
      entityWithAppearance(components, played.entityId);
  if (!body.has_value()) {
    return;
  }
  const game::Position* const position = components.get<game::Position>(*body);
  const EntityAppearance& appearance =
      *components.get<AppearanceReference>(*body)->appearance;
  const NamedEffect* const effect = effectNamed(appearance, played.effect);
  if (position == nullptr || effect == nullptr) {
    return;
  }
  const EffectAnimations animations = animationsOf(*effect);
  spawnSheetEffect(tools_, SheetEffect{.animations = &animations,
                                       .center = *position,
                                       .hold = {},
                                       .flash = {},
                                       .sheet = appearance.sheet});
}

}  // namespace rtype::client

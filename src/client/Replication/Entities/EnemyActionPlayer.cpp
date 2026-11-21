#include "EnemyActionPlayer.hpp"
#include <algorithm>
#include <optional>
#include <string>
#include "ActionAnimation.hpp"
#include "ActionKey.hpp"
#include "ActionSound.hpp"
#include "Animation.hpp"
#include "AppearanceReference.hpp"
#include "ComponentRegistry.hpp"
#include "EnemyAction.hpp"
#include "Enraged.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "EntityWithAppearance.hpp"
#include "ISoundPlayer.hpp"
#include "SheetPhase.hpp"
#include "ShownLooks.hpp"
#include "SpriteAnimation.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The animation @p appearance gives @p key, or nothing when the
 * sheet gives none. */
const ActionAnimation* actionOf(const EntityAppearance& appearance,
                                game::ActionKey key) {
  const auto found =
      std::ranges::find(appearance.actions, key, &ActionAnimation::key);
  return found == appearance.actions.end() ? nullptr : &*found;
}

/** @returns What an enemy plays for @p action: its frames from the first,
 * then @p idle when the sheet says so. */
SpriteAnimation playingOf(const ActionAnimation& action,
                          const Animation& idle) {
  return SpriteAnimation{
      .animation = Animation{.frames = action.frames,
                             .frameDuration = action.frameDuration,
                             .playback = action.playback,
                             .anchor = action.anchor},
      .elapsed = engine::Duration::zero(),
      .afterwards =
          action.returnsToIdle ? std::optional<Animation>{idle} : std::nullopt};
}

/** @returns The sound @p appearance names for @p key, or nullptr when the
 * sheet names none. */
const std::string* actionSoundOf(const EntityAppearance& appearance,
                                 game::ActionKey key) {
  const auto found =
      std::ranges::find(appearance.sounds.actions, key, &ActionSound::key);
  return found == appearance.sounds.actions.end() ? nullptr : &found->sound;
}

}  // namespace

EnemyActionPlayer::EnemyActionPlayer(engine::ComponentRegistry& components,
                                     ISoundPlayer& sounds)
    : components_(&components), sounds_(&sounds) {}

void EnemyActionPlayer::apply(const network::EnemyAction& action) {
  const std::optional<engine::Entity> enemy =
      entityWithAppearance(*components_, action.entityId);
  if (!enemy.has_value()) {
    return;
  }
  const bool rage = action.phase == network::EnemyActionPhase::Enraged;
  if (rage) {
    components_->add(*enemy, Enraged{});
  }
  const EntityAppearance& appearance =
      *components_->get<AppearanceReference>(*enemy)->appearance;
  const game::ActionKey key{.attack = action.attack,
                            .phase = sheetPhaseOf(action.phase)};
  if (const std::string* sound = actionSoundOf(appearance, key)) {
    sounds_->play(*sound);
  }
  auto* const playing = components_->get<SpriteAnimation>(*enemy);
  if (playing == nullptr) {
    return;
  }
  const Animation idle = restingAnimationOf(*components_, *enemy, appearance);
  const ActionAnimation* const found = actionOf(appearance, key);
  if (found != nullptr) {
    *playing = playingOf(*found, idle);
  } else if (rage) {
    *playing = SpriteAnimation{.animation = idle};
  }
}

}  // namespace rtype::client

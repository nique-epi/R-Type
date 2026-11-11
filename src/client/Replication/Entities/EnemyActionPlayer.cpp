#include "EnemyActionPlayer.hpp"
#include <algorithm>
#include <optional>
#include "ActionAnimation.hpp"
#include "ActionKey.hpp"
#include "Animation.hpp"
#include "AppearanceReference.hpp"
#include "ComponentRegistry.hpp"
#include "EnemyAction.hpp"
#include "EnemyPhase.hpp"
#include "Enraged.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "EntityWithAppearance.hpp"
#include "ShownLooks.hpp"
#include "SpriteAnimation.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The phase of the sheets that the phase of the wire names. */
game::EnemyPhase phaseOf(network::EnemyActionPhase phase) {
  switch (phase) {
    case network::EnemyActionPhase::Signal:
      return game::EnemyPhase::Signal;
    case network::EnemyActionPhase::Strike:
      return game::EnemyPhase::Strike;
    case network::EnemyActionPhase::Recover:
      return game::EnemyPhase::Recover;
    case network::EnemyActionPhase::Enraged:
      return game::EnemyPhase::Enraged;
  }
  return game::EnemyPhase::Signal;
}

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

}  // namespace

EnemyActionPlayer::EnemyActionPlayer(engine::ComponentRegistry& components)
    : components_(&components) {}

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
  auto* const playing = components_->get<SpriteAnimation>(*enemy);
  if (playing == nullptr) {
    return;
  }
  const EntityAppearance& appearance =
      *components_->get<AppearanceReference>(*enemy)->appearance;
  const Animation idle = restingAnimationOf(*components_, *enemy, appearance);
  const ActionAnimation* const found = actionOf(
      appearance,
      game::ActionKey{.attack = action.attack, .phase = phaseOf(action.phase)});
  if (found != nullptr) {
    *playing = playingOf(*found, idle);
  } else if (rage) {
    *playing = SpriteAnimation{.animation = idle};
  }
}

}  // namespace rtype::client

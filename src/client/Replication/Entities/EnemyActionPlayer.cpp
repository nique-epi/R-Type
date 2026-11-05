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
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "EntitySprites.hpp"
#include "NetworkIdentity.hpp"
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

/** @returns What an entity of @p appearance plays for @p action: its frames
 * from the first, and the idle animation after them when the sheet says so. */
SpriteAnimation playingOf(const EntityAppearance& appearance,
                          const ActionAnimation& action) {
  return SpriteAnimation{
      .animation = Animation{.frames = action.frames,
                             .frameDuration = action.frameDuration,
                             .playback = action.playback,
                             .anchor = action.anchor},
      .elapsed = engine::Duration::zero(),
      .afterwards = action.returnsToIdle
                        ? std::optional<Animation>{idleAnimationOf(appearance)}
                        : std::nullopt};
}

}  // namespace

EnemyActionPlayer::EnemyActionPlayer(engine::ComponentRegistry& components)
    : components_(&components) {}

void EnemyActionPlayer::apply(const network::EnemyAction& action) {
  const game::ActionKey key{.attack = action.attack,
                            .phase = phaseOf(action.phase)};
  components_->forEach<game::NetworkIdentity, AppearanceReference>(
      [this, key, &action](engine::Entity entity,
                           const game::NetworkIdentity& identity,
                           const AppearanceReference& look) {
        if (identity.networkId != action.entityId ||
            look.appearance == nullptr) {
          return;
        }
        const ActionAnimation* const found = actionOf(*look.appearance, key);
        auto* const playing = components_->get<SpriteAnimation>(entity);
        if (found != nullptr && playing != nullptr) {
          *playing = playingOf(*look.appearance, *found);
        }
      });
}

}  // namespace rtype::client

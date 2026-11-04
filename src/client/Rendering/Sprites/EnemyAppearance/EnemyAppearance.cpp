#include "EnemyAppearance.hpp"
#include <algorithm>
#include <optional>
#include <utility>
#include "ActionKey.hpp"
#include "ComponentRegistry.hpp"
#include "EnemyKind.hpp"
#include "Entity.hpp"
#include "EntitySpriteConstants.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

constexpr EnemyAppearance BYDO_APPEARANCE{
    .sheetId = ENTITY_SHEET_ID, .idle = BYDO_ANIMATION, .actions = {}};

}  // namespace

const EnemyAppearance& appearanceOf(game::EnemyKind kind) {
  switch (kind) {
    case game::EnemyKind::Drifter:
    case game::EnemyKind::Weaver:
    case game::EnemyKind::Voltjelly:
    case game::EnemyKind::Reaper:
    case game::EnemyKind::Bydomancer:
    case game::EnemyKind::Drillback:
    case game::EnemyKind::BydoTyrant:
      return BYDO_APPEARANCE;
  }
  return BYDO_APPEARANCE;
}

std::optional<SpriteAnimation> startedActionOf(
    const EnemyAppearance& appearance, game::ActionKey action) {
  const auto found = std::ranges::find_if(
      appearance.actions, [action](const ActionLook& look) {
        return look.attack == action.attack && look.phase == action.phase;
      });
  if (found == appearance.actions.end()) {
    return std::nullopt;
  }
  const bool playsOnce = found->animation.playback == Animation::Playback::Once;
  return SpriteAnimation{
      .animation = found->animation,
      .elapsed = engine::Duration::zero(),
      .afterwards = playsOnce ? std::optional{appearance.idle} : std::nullopt};
}

void showEnemyAction(engine::ComponentRegistry& components,
                     engine::Entity enemy, game::ActionKey action) {
  const game::EnemyKind* const kind = components.get<game::EnemyKind>(enemy);
  game::Sprite* const sprite = components.get<game::Sprite>(enemy);
  SpriteAnimation* const animation = components.get<SpriteAnimation>(enemy);
  if (kind == nullptr || sprite == nullptr || animation == nullptr) {
    return;
  }
  std::optional<SpriteAnimation> started =
      startedActionOf(appearanceOf(*kind), action);
  if (!started.has_value()) {
    return;
  }
  sprite->frame =
      started->animation.frameAt(started->elapsed).value_or(sprite->frame);
  *animation = std::move(*started);
}

}  // namespace rtype::client

#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include "ActionKey.hpp"
#include "Animation.hpp"
#include "EnemyKind.hpp"
#include "EnemyPhase.hpp"
#include "Entity.hpp"
#include "SpriteAnimation.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::client {

/** @brief The animation an enemy plays in one phase of one attack. */
struct ActionLook {
  std::uint8_t attack;
  game::EnemyPhase phase;
  Animation animation;
};

/** @brief How a kind of enemy looks: the sheet its frames lie on, its idle
 * loop, and one animation per (attack, phase) it has one for. */
struct EnemyAppearance {
  const char* sheetId;
  Animation idle;
  std::span<const ActionLook> actions;
};

/**
 * @returns The appearance of @p kind. Every kind but the Drifter and the
 * Weaver is drawn as a Bydo until its own pull request fills its row.
 */
[[nodiscard]] const EnemyAppearance& appearanceOf(game::EnemyKind kind);

/**
 * @returns The animation of @p appearance for @p action, started at zero. An
 * animation played once is followed by the idle; a Hold or a Loop is not.
 * Nothing when the appearance has no animation for that action.
 */
[[nodiscard]] std::optional<SpriteAnimation> startedActionOf(
    const EnemyAppearance& appearance, game::ActionKey action);

/**
 * @brief Shows @p action on @p enemy: replaces its animation and the frame of
 * its Sprite by those of its kind, when the enemy carries a kind, a Sprite and
 * a SpriteAnimation and its appearance has the action. Does nothing otherwise.
 */
void showEnemyAction(engine::ComponentRegistry& components,
                     engine::Entity enemy, game::ActionKey action);

}  // namespace rtype::client

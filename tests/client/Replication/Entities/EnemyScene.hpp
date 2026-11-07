#pragma once

#include <cstdint>
#include "AnimationSystem.hpp"
#include "ComponentRegistry.hpp"
#include "EnemyAction.hpp"
#include "EnemyActionPlayer.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "EntityRegistry.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief An enemy that shows its idle frame, an animation system, and the
 * player of the actions announced to it. */
class EnemyScene {
 public:
  EnemyScene();

  /** @brief Hands the player the action @p attack in @p phase of the enemy
   * @p entityId. */
  void announce(std::uint32_t entityId, std::uint8_t attack,
                network::EnemyActionPhase phase);
  /** @brief Runs the animation system for @p elapsed. */
  void play(engine::Duration elapsed);
  [[nodiscard]] const SpriteAnimation& playing();
  [[nodiscard]] game::SpriteFrame shownFrame();

 private:
  EntityAppearance appearance_;
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  engine::Entity enemy_{};
  EnemyActionPlayer player_{components_};
  AnimationSystem system_;
};

}  // namespace rtype::client::testing

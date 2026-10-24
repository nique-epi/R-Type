#pragma once

#include "AnimationSystem.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

/** @brief Registries holding one entity, showing FIRST and an animation, and
 * the system that plays it. */
class AnimatedScene {
 public:
  explicit AnimatedScene(const rtype::client::SpriteAnimation& animation);

  void play(rtype::engine::Duration elapsed);

  /** @returns The frame the sprite of the entity shows, NO_FRAME when it has
   * no Sprite. */
  [[nodiscard]] rtype::game::SpriteFrame shownFrame();

  [[nodiscard]] bool isAlive() const;

 private:
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_;
  rtype::engine::Entity entity_;
  rtype::client::AnimationSystem system_;
};

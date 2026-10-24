#pragma once

#include "Sprite.hpp"
#include "SpriteAnimation.hpp"

namespace rtype::client {

/** @brief The look of a new entity: its sprite, already showing the frame its
 * animation starts on, and that animation. */
struct AnimatedSprite {
  game::Sprite sprite;
  SpriteAnimation animation;
};

}  // namespace rtype::client

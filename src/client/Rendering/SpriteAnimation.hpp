#pragma once

#include "Animation.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Component of an entity whose Sprite shows an animation: the
 * animation, and the time since it started.
 *
 * AnimationSystem moves `elapsed` forward and writes the frame the animation
 * gives into the Sprite of the entity. An entity whose animation played once
 * has ended is destroyed: that is how an effect removes itself. A loop may
 * start with `elapsed` already past zero, so entities created together do not
 * show the same frame.
 */
struct SpriteAnimation {
  Animation animation;
  engine::Duration elapsed{};
};

}  // namespace rtype::client

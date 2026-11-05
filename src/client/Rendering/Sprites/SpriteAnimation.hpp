#pragma once

#include <optional>
#include "Animation.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Component of an entity whose Sprite shows an animation: the
 * animation, and the time since it started.
 *
 * AnimationSystem moves `elapsed` forward and writes the frame the animation
 * gives into the Sprite of the entity. An entity whose animation played once
 * has ended is destroyed, unless `afterwards` names the animation that takes
 * its place: that is how an effect removes itself, and how an attack goes back
 * to the idle animation. A loop may
 * start with `elapsed` already past zero, so entities created together do not
 * show the same frame.
 */
struct SpriteAnimation {
  Animation animation;
  engine::Duration elapsed{};
  /** @brief The animation that replaces `animation` once it has ended, started
   * from its first frame; none when nothing follows. */
  std::optional<Animation> afterwards = std::nullopt;
};

}  // namespace rtype::client

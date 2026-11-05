#pragma once

#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::client {

/**
 * @brief Plays the animation of every entity that has a SpriteAnimation and a
 * Sprite.
 *
 * Each step moves the time of each animation forward by the time the step
 * covers, then writes the frame the animation gives into the Sprite: the frame
 * follows the clock, however many pictures are drawn per second. When an
 * animation has ended, the animation named by `afterwards` takes its place,
 * from its first frame; with none, a held animation keeps its last frame and an
 * animation played once destroys its entity, so an effect removes itself after
 * its last frame; the destruction waits until the walk over the components
 * ends, and the entity is not drawn again.
 */
class AnimationSystem final : public engine::ISystem {
 public:
  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;
};

}  // namespace rtype::client

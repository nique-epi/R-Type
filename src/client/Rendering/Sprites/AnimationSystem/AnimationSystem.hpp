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
 * follows the clock, however many pictures are drawn per second. An entity
 * whose animation played once has ended is destroyed, so an effect removes
 * itself after its last frame; the destruction waits until the walk over the
 * components ends, and the entity is not drawn again.
 */
class AnimationSystem final : public engine::ISystem {
 public:
  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;
};

}  // namespace rtype::client

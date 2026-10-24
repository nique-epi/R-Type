#pragma once

#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::client {

/**
 * @brief Moves the pixel effects forward: particles fly and slow down,
 * shockwaves and speed lines age, and each is destroyed once its life is
 * spent, which waits until the walk over the components ends.
 *
 * A particle whose age is still negative after the step neither moves nor
 * slows: it waits for the end of a hold. Its speed keeps
 * PARTICLE_SPEED_KEPT_PER_TICK of itself over each SIMULATION_TICK_DURATION,
 * whatever the length of the step.
 */
class GroundEffectSystem final : public engine::ISystem {
 public:
  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  /** @brief Moves every particle whose age is not negative along its
   * speed, then slows it down. */
  static void moveParticles(engine::ComponentRegistry& components,
                            engine::Duration elapsed);
};

}  // namespace rtype::client

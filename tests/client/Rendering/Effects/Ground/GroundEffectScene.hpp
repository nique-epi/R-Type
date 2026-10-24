#pragma once

#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "GroundEffectSystem.hpp"
#include "TimeConstants.hpp"

/**
 * @brief An entity registry, its components and the system of the ground
 * effects, with a way to add particles.
 */
class GroundEffectScene {
 public:
  GroundEffectScene();

  /** @returns A particle at START, leaving at SPEED to the right, aged
   * @p age, living A_PARTICLE_LIFE. */
  rtype::engine::Entity addParticle(rtype::engine::Duration age);

  void run(rtype::engine::Duration elapsed);

  rtype::engine::ComponentRegistry& components();

  [[nodiscard]] bool isAlive(rtype::engine::Entity entity) const;

 private:
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_;
  rtype::client::GroundEffectSystem system_;
};

#pragma once

#include <gtest/gtest.h>
#include <vector>
#include "ColoredPixel.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "Particle.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

/**
 * @brief An entity registry and its components, with a way to add ground
 * effects and to read the pixels paintGroundEffects() paints for them.
 */
class GroundEffectsFixture : public ::testing::Test {
 public:
  GroundEffectsFixture();

 protected:
  /** @brief Adds an entity at @p position carrying @p effect. */
  template <typename Effect>
  void addEffect(rtype::game::Position position, Effect effect) {
    components_.add(add(position), effect);
  }

  /** @returns The pixels paintGroundEffects() paints, in order. */
  std::vector<rtype::client::ColoredPixel> paint();

  /** @returns A drop that shrinks, living A_PARTICLE_LIFE, aged @p age. */
  static rtype::client::Particle shrinkingDropAged(rtype::engine::Duration age);

 private:
  rtype::engine::Entity add(rtype::game::Position position);

  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_;
};

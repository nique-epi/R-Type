#pragma once

#include <SFML/Graphics/Color.hpp>
#include <utility>
#include <vector>
#include "ComponentRegistry.hpp"
#include "EntityRegistry.hpp"
#include "Particle.hpp"
#include "PixelEffects.hpp"
#include "Shockwave.hpp"

/**
 * @brief An entity registry, its components and the pixel effects that add
 * to them, seeded with EFFECT_TEST_SEED, with a way to read what they added.
 */
class PixelScene {
 public:
  PixelScene();

  rtype::client::PixelEffects& pixels();

  std::vector<rtype::client::Particle> particles();

  std::vector<rtype::client::Shockwave> shockwaves();

  /** @returns Each speed line with the row it lies on, from the top. */
  std::vector<std::pair<float, sf::Color>> speedLines();

 private:
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_;
  rtype::client::PixelEffects pixels_;
};

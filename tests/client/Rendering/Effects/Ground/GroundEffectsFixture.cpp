#include "GroundEffectsFixture.hpp"
#include <vector>
#include "ColoredPixel.hpp"
#include "EffectTestConstants.hpp"
#include "Entity.hpp"
#include "GroundEffects.hpp"
#include "GroundEffectsTestConstants.hpp"
#include "Particle.hpp"
#include "Position.hpp"
#include "RecordingPixelSurface.hpp"
#include "TimeConstants.hpp"

using rtype::client::ColoredPixel;
using rtype::client::paintGroundEffects;
using rtype::client::Particle;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::game::Position;

GroundEffectsFixture::GroundEffectsFixture() : components_(entities_) {}

std::vector<ColoredPixel> GroundEffectsFixture::paint() {
  RecordingPixelSurface surface;
  paintGroundEffects(components_, surface);
  return surface.plots();
}

Particle GroundEffectsFixture::shrinkingDropAged(Duration age) {
  return Particle{.velocity = {},
                  .age = age,
                  .life = A_PARTICLE_LIFE,
                  .colors = RAMP,
                  .shrinks = true};
}

Entity GroundEffectsFixture::add(Position position) {
  const Entity entity = entities_.create();
  components_.add(entity, position);
  return entity;
}

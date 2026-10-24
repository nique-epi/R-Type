#include "GroundEffectScene.hpp"
#include "ComponentRegistry.hpp"
#include "EffectTestConstants.hpp"
#include "Entity.hpp"
#include "GroundEffectSystemTestConstants.hpp"
#include "Particle.hpp"
#include "TimeConstants.hpp"

using rtype::client::Particle;
using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::Entity;

GroundEffectScene::GroundEffectScene() : components_(entities_) {}

Entity GroundEffectScene::addParticle(Duration age) {
  const Entity entity = entities_.create();
  components_.add(entity, START);
  components_.add(entity, Particle{.velocity = {SPEED, 0.0F},
                                   .age = age,
                                   .life = A_PARTICLE_LIFE,
                                   .colors = RAMP,
                                   .shrinks = false});
  return entity;
}

void GroundEffectScene::run(Duration elapsed) {
  system_.update(components_, elapsed);
}

ComponentRegistry& GroundEffectScene::components() { return components_; }

bool GroundEffectScene::isAlive(Entity entity) const {
  return entities_.isAlive(entity);
}

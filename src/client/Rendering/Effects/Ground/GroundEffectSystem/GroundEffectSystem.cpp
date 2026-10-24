#include "GroundEffectSystem.hpp"
#include <chrono>
#include <cmath>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "GroundEffectConstants.hpp"
#include "Particle.hpp"
#include "Position.hpp"
#include "Shockwave.hpp"
#include "SpeedLine.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The share of its speed a particle keeps over @p seconds. */
float speedKeptOver(float seconds) {
  const float ticks =
      seconds /
      std::chrono::duration<float>(engine::SIMULATION_TICK_DURATION).count();
  return std::pow(PARTICLE_SPEED_KEPT_PER_TICK, ticks);
}

/** @brief Ages every effect of that type by the elapsed time, and destroys
 * the entity of each one whose life is spent. */
template <typename Effect>
void ageEffects(engine::ComponentRegistry& components,
                engine::Duration elapsed) {
  components.forEach<Effect, Effect>(
      [&components, elapsed](engine::Entity entity, Effect& effect, Effect&) {
        effect.age += elapsed;
        if (effect.age >= effect.life) {
          components.destroy(entity);
        }
      });
}

}  // namespace

void GroundEffectSystem::update(engine::ComponentRegistry& components,
                                engine::Duration elapsed) {
  ageEffects<Particle>(components, elapsed);
  moveParticles(components, elapsed);
  ageEffects<Shockwave>(components, elapsed);
  ageEffects<SpeedLine>(components, elapsed);
}

void GroundEffectSystem::moveParticles(engine::ComponentRegistry& components,
                                       engine::Duration elapsed) {
  const float seconds = std::chrono::duration<float>(elapsed).count();
  const float speedKept = speedKeptOver(seconds);
  components.forEach<Particle, game::Position>(
      [seconds, speedKept](engine::Entity, Particle& particle,
                           game::Position& position) {
        if (particle.age < engine::Duration::zero()) {
          return;
        }
        position.x += particle.velocity.x * seconds;
        position.y += particle.velocity.y * seconds;
        particle.velocity *= speedKept;
      });
}

}  // namespace rtype::client

#include "PixelEffects.hpp"
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include "ArrivalLines.hpp"
#include "BackgroundConstants.hpp"
#include "Burst.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "GroundEffectConstants.hpp"
#include "Particle.hpp"
#include "PlacedBurst.hpp"
#include "PlacedRing.hpp"
#include "Position.hpp"
#include "RandomRange.hpp"
#include "RenderingConstants.hpp"
#include "Shockwave.hpp"
#include "SpeedLine.hpp"
#include "SpriteToneColorConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

PixelEffects::PixelEffects(engine::EntityRegistry& entities,
                           engine::ComponentRegistry& components,
                           std::uint32_t seed)
    : entities_(&entities), components_(&components), random_(seed) {}

void PixelEffects::burst(const PlacedBurst& particles) {
  const std::size_t room =
      PARTICLE_MAXIMUM - std::min(particleCount(), PARTICLE_MAXIMUM);
  const std::size_t count = std::min(particles.shape.count, room);
  for (std::size_t i = 0; i < count; ++i) {
    addParticle(particles);
  }
}

void PixelEffects::wave(const PlacedRing& wave) {
  const engine::Entity entity = addEntityAt(wave.center);
  components_->add(entity, Shockwave{.radius = wave.shape.radius,
                                     .age = -wave.delay,
                                     .life = wave.shape.life,
                                     .colors = wave.colors});
}

void PixelEffects::speedLines(const ArrivalLines& lines) {
  for (std::size_t i = 0; i < SPEED_LINE_ROWS.size(); ++i) {
    const auto row = static_cast<float>(SPEED_LINE_ROWS[i]);
    const engine::Entity entity = addEntityAt(game::Position{
        .x = lines.shipCenter.x,
        .y = lines.shipCenter.y + (row * BACKGROUND_PIXEL_SIZE)});
    const bool white = lines.flashes && i % 2 == 0;
    components_->add(entity,
                     SpeedLine{.age = -lines.delay,
                               .life = lines.life,
                               .color = white ? FLASH_COLOR : lines.light});
  }
}

std::size_t PixelEffects::particleCount() {
  std::size_t count = 0;
  components_->forEach<Particle, Particle>(
      [&count](engine::Entity, Particle&, Particle&) { ++count; });
  return count;
}

void PixelEffects::addParticle(const PlacedBurst& particles) {
  const Burst& shape = particles.shape;
  const float direction =
      particles.aim +
      randomBetween(random_, -shape.cone * HALF, shape.cone * HALF);
  const float speed = randomBetween(random_, shape.slowest, shape.fastest);
  const float lifeScale =
      1.0F + randomBetween(random_, -shape.lifeSpread, shape.lifeSpread);
  const engine::Entity entity = addEntityAt(particles.center);
  components_->add(
      entity, Particle{.velocity = sf::Vector2f(speed, sf::radians(direction)),
                       .age = -particles.delay,
                       .life = std::chrono::duration_cast<engine::Duration>(
                           shape.life * lifeScale),
                       .colors = particles.colors,
                       .shrinks = shape.shrinks});
}

engine::Entity PixelEffects::addEntityAt(game::Position position) {
  const engine::Entity entity = entities_->create();
  components_->add(entity, position);
  return entity;
}

}  // namespace rtype::client

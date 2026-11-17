#include "ParticleLayer.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include "BackgroundConstants.hpp"
#include "ILayerCanvas.hpp"
#include "LayerFrame.hpp"
#include "PaintedParticle.hpp"
#include "Particle.hpp"
#include "ParticleContent.hpp"
#include "ParticleLayerParts.hpp"
#include "ParticleMotion.hpp"
#include "ParticlePainting.hpp"
#include "StarTwinkleStep.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ParticleLayer::ParticleLayer(ParticleLayerParts parts)
    : description_(std::move(parts.description)),
      repeat_(static_cast<int>(parts.repeat)),
      random_(parts.seed) {
  particles_.reserve(description_.population);
  for (std::size_t index = 0; index < description_.population; ++index) {
    particles_.push_back(settledParticle(description_, random_));
  }
}

void ParticleLayer::advance(engine::Duration elapsed, const LayerFrame& frame) {
  age_ += elapsed;
  if (description_.population > 0) {
    bringBack(frame.column);
    return;
  }
  const float seconds = std::chrono::duration<float>(elapsed).count();
  for (Particle& particle : particles_) {
    moveParticle(particle, description_, seconds);
  }
  std::erase_if(particles_, [this, &frame](const Particle& particle) {
    return hasEnded(particle, description_, frame.column);
  });
}

void ParticleLayer::paint(ILayerCanvas& canvas, const LayerFrame& frame) const {
  const std::int64_t step =
      std::chrono::duration_cast<StarTwinkleStep>(age_).count();
  for (const Particle& particle : particles_) {
    const bool twinkling =
        description_.twinkleSteps > 0 &&
        (step + particle.twinklePhase) % description_.twinkleSteps == 0;
    paintParticle(canvas, description_,
                  PaintedParticle{.particle = particle,
                                  .place = placeOf(particle, frame.column),
                                  .streak = frame.streak,
                                  .twinkling = twinkling});
  }
}

int ParticleLayer::leftEdge() const {
  return static_cast<int>(std::floor(description_.area.position.x));
}

void ParticleLayer::emit() {
  particles_.push_back(emittedParticle(description_, random_));
}

std::size_t ParticleLayer::aliveCount() const { return particles_.size(); }

const std::vector<Particle>& ParticleLayer::particles() const {
  return particles_;
}

sf::Vector2f ParticleLayer::placeOf(const Particle& particle,
                                    int column) const {
  const auto moved = static_cast<float>(column);
  if (description_.population == 0) {
    return particle.position + sf::Vector2f(moved, 0.0F);
  }
  const int lap = lapOf(particle.position.x, column);
  return {particle.position.x + moved - static_cast<float>(lap * repeat_),
          particle.position.y};
}

int ParticleLayer::lapOf(float home, int column) const {
  if (repeat_ <= 0) {
    return 0;
  }
  const int hidden =
      std::max(0, (repeat_ - static_cast<int>(BACKGROUND_WIDTH)) / 2);
  const double passed = static_cast<double>(home) + column + hidden;
  return static_cast<int>(std::floor(passed / repeat_));
}

void ParticleLayer::bringBack(int column) {
  for (Particle& particle : particles_) {
    const int lap = lapOf(particle.position.x, column);
    if (lap != particle.lap) {
      particle.lap = lap;
      particle.position.y = drawnHeight(description_, random_);
    }
  }
}

}  // namespace rtype::client

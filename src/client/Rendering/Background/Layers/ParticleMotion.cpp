#include "ParticleMotion.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>
#include "BackgroundConstants.hpp"
#include "DecimalRange.hpp"
#include "DurationRange.hpp"
#include "Particle.hpp"
#include "ParticleContent.hpp"
#include "ParticleSprite.hpp"
#include "ParticleTrail.hpp"
#include "RandomRange.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns A number of @p range, drawn from @p random. */
float drawnIn(engine::DecimalRange range, std::mt19937& random) {
  return randomBetween(random, static_cast<float>(range.lowest),
                       static_cast<float>(range.highest));
}

/** @returns @p duration in seconds. */
float secondsOf(engine::Duration duration) {
  return std::chrono::duration<float>(duration).count();
}

/** @returns A point of the area of @p description, drawn from @p random. */
sf::Vector2f drawnPoint(const ParticleContent& description,
                        std::mt19937& random) {
  const sf::FloatRect& area = description.area;
  return randomPointBetween(random, area.position, area.position + area.size);
}

/** @returns The rank of one of the sprites of @p description, drawn from
 * @p random by their weights; 0 when it has one sprite or none. */
std::size_t drawnSprite(const ParticleContent& description,
                        std::mt19937& random) {
  if (description.sprites.size() < 2) {
    return 0;
  }
  std::vector<double> weights;
  weights.reserve(description.sprites.size());
  for (const ParticleSprite& sprite : description.sprites) {
    weights.push_back(sprite.weight);
  }
  std::discrete_distribution<std::size_t> ranks(weights.begin(), weights.end());
  return ranks(random);
}

/** @brief Draws from @p random the sprite @p particle is drawn with and,
 * when that sprite loops, its frame rate and the frame it starts on. */
void drawSprite(const ParticleContent& description, Particle& particle,
                std::mt19937& random) {
  particle.sprite = drawnSprite(description, random);
  if (description.sprites.empty()) {
    return;
  }
  const ParticleSprite& sprite = description.sprites[particle.sprite];
  if (sprite.framesPerSecond.has_value()) {
    particle.frameRate = drawnIn(*sprite.framesPerSecond, random);
    particle.frame =
        randomBetween(random, 0.0F, static_cast<float>(sprite.frames));
  }
}

/** @returns Whether @p particle has a life and has lived it. */
bool hasLivedItsLife(const Particle& particle) {
  return particle.life > 0.0F && particle.age >= particle.life;
}

/** @returns Whether @p place, on the background, is more than @p margin
 * pixels out of it. */
bool isOutOfTheBackground(sf::Vector2f place, float margin) {
  return place.x < -margin || place.x > BACKGROUND_WIDTH + margin ||
         place.y < -margin || place.y > BACKGROUND_HEIGHT + margin;
}

}  // namespace

Particle settledParticle(const ParticleContent& description,
                         std::mt19937& random) {
  Particle particle{};
  const sf::Vector2f point = drawnPoint(description, random);
  particle.position = {std::floor(point.x), point.y};
  if (description.twinkleSteps > 0) {
    std::uniform_int_distribution<std::int64_t> phases(
        0, description.twinkleSteps - 1);
    particle.twinklePhase = phases(random);
  }
  drawSprite(description, particle, random);
  return particle;
}

Particle emittedParticle(const ParticleContent& description,
                         std::mt19937& random) {
  Particle particle{};
  particle.position = drawnPoint(description, random);
  const float speed = drawnIn(description.speed, random);
  const sf::Angle angle = sf::degrees(drawnIn(description.angle, random));
  particle.velocity =
      sf::Vector2f(speed, angle) +
      sf::Vector2f(
          0.0F, randomBetween(random, -description.drift, description.drift));
  if (description.life.has_value()) {
    particle.life = randomBetween(random, secondsOf(description.life->shortest),
                                  secondsOf(description.life->longest));
  }
  drawSprite(description, particle, random);
  return particle;
}

float drawnHeight(const ParticleContent& description, std::mt19937& random) {
  const sf::FloatRect& area = description.area;
  return randomBetween(random, area.position.y, area.position.y + area.size.y);
}

void moveParticle(Particle& particle, const ParticleContent& description,
                  float seconds) {
  particle.position += particle.velocity * seconds;
  particle.age += seconds;
  particle.frame += particle.frameRate * seconds;
  if (!description.trail.has_value()) {
    return;
  }
  const ParticleTrail& trail = *description.trail;
  if (hasLivedItsLife(particle)) {
    particle.trailLength -= trail.shrink * seconds;
  } else {
    particle.trailLength = std::min(
        trail.longest, particle.trailLength + (trail.growth * seconds));
  }
}

bool hasEnded(const Particle& particle, const ParticleContent& description,
              int column) {
  if (particle.life > 0.0F) {
    return hasLivedItsLife(particle) &&
           (!description.trail.has_value() || particle.trailLength <= 0.0F);
  }
  return isOutOfTheBackground(
      particle.position + sf::Vector2f(static_cast<float>(column), 0.0F),
      description.margin);
}

}  // namespace rtype::client

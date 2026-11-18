#include "ParticlePainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include "ILayerCanvas.hpp"
#include "LayerParticle.hpp"
#include "PaintedParticle.hpp"
#include "ParticleContent.hpp"
#include "ParticleSprite.hpp"
#include "ParticleTrail.hpp"
#include "PixelRounding.hpp"
#include "StripFrame.hpp"
#include "TrailStop.hpp"

namespace rtype::client {

namespace {

/** @returns The color of @p trail at @p share of its length: that of the
 * first stop that ends beyond it, of the last when none does, and
 * @p otherwise for a trail without stops. */
sf::Color trailColorAt(const ParticleTrail& trail, float share,
                       sf::Color otherwise) {
  for (const TrailStop& stop : trail.stops) {
    if (share < stop.until) {
      return stop.color;
    }
  }
  return trail.stops.empty() ? otherwise : trail.stops.back().color;
}

/** @brief Paints the streak @p painted leaves to its right while its layer
 * warps, in the warp color of @p description; nothing without that color. */
void paintStreak(ILayerCanvas& canvas, const ParticleContent& description,
                 const PaintedParticle& painted) {
  if (!description.warpTrail.has_value()) {
    return;
  }
  const sf::Vector2i pixel = nearestPixel(painted.place);
  for (int column = 1; column <= painted.streak; ++column) {
    canvas.plot(pixel + sf::Vector2i(column, 0), *description.warpTrail);
  }
}

/** @brief Paints the trail of @p painted behind it, along its path, from
 * its second pixel: the first is the particle. Nothing for a layer without a
 * trail, or a particle that stays. */
void paintTrail(ILayerCanvas& canvas, const ParticleContent& description,
                const PaintedParticle& painted) {
  const sf::Vector2f velocity = painted.particle.velocity;
  if (!description.trail.has_value() || velocity == sf::Vector2f()) {
    return;
  }
  const sf::Vector2f direction = velocity.normalized();
  const int pixels =
      std::max(1, static_cast<int>(std::lround(painted.particle.trailLength)));
  const sf::Color otherwise = description.pixel.value_or(sf::Color::White);
  for (int pixel = 1; pixel < pixels; ++pixel) {
    const float share = static_cast<float>(pixel) / static_cast<float>(pixels);
    canvas.plot(
        nearestPixel(painted.place - (direction * static_cast<float>(pixel))),
        trailColorAt(*description.trail, share, otherwise));
  }
}

/** @returns The frame of @p sprite @p painted shows: the one its loop is
 * on, its share of its life, or its second while it twinkles. */
std::size_t shownFrame(const ParticleSprite& sprite,
                       const PaintedParticle& painted) {
  const LayerParticle& particle = painted.particle;
  if (sprite.framesPerSecond.has_value()) {
    return static_cast<std::size_t>(particle.frame) % sprite.frames;
  }
  if (particle.life > 0.0F) {
    const auto lived = static_cast<std::size_t>(
        particle.age / particle.life * static_cast<float>(sprite.frames));
    return std::min(sprite.frames - 1, lived);
  }
  return painted.twinkling && sprite.frames > 1 ? 1 : 0;
}

/** @brief Paints the particle itself: its pixel, or the frame of its
 * sprite, its anchor on the particle. */
void paintBody(ILayerCanvas& canvas, const ParticleContent& description,
               const PaintedParticle& painted) {
  const sf::Vector2i pixel = nearestPixel(painted.place);
  if (description.pixel.has_value()) {
    const sf::Color twinkle = description.twinkle.value_or(*description.pixel);
    canvas.plot(pixel, painted.twinkling ? twinkle : *description.pixel);
    return;
  }
  if (painted.particle.sprite >= description.sprites.size()) {
    return;
  }
  const ParticleSprite& sprite = description.sprites[painted.particle.sprite];
  canvas.drawFrame(StripFrame{.file = sprite.file,
                              .frames = sprite.frames,
                              .frame = shownFrame(sprite, painted)},
                   pixel - nearestPixel(sprite.anchor));
}

}  // namespace

void paintParticle(ILayerCanvas& canvas, const ParticleContent& description,
                   const PaintedParticle& painted) {
  paintStreak(canvas, description, painted);
  paintTrail(canvas, description, painted);
  paintBody(canvas, description, painted);
}

}  // namespace rtype::client

#include "GroundEffects.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <span>
#include <vector>
#include "BackgroundConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Easing.hpp"
#include "Entity.hpp"
#include "GroundEffectConstants.hpp"
#include "IPixelSurface.hpp"
#include "Particle.hpp"
#include "PixelRounding.hpp"
#include "Position.hpp"
#include "Shockwave.hpp"
#include "SpeedLine.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The share of its life an effect has lived, 0 to 1. */
float shareOfLife(engine::Duration age, engine::Duration life) {
  if (life <= engine::Duration::zero()) {
    return 1.0F;
  }
  return std::clamp(shareOf(age, life), 0.0F, 1.0F);
}

/** @returns The color of @p colors that a share of life reaches: the first
 * for 0, the last one for the end. */
sf::Color colorAt(std::span<const sf::Color> colors, float share) {
  const auto index =
      static_cast<std::size_t>(share * static_cast<float>(colors.size()));
  return colors[std::min(index, colors.size() - 1)];
}

/** @returns The pixel of the canvas under a position of the playfield. */
sf::Vector2i pixelOf(game::Position position) {
  return nearestPixel(sf::Vector2f(position.x, position.y) /
                      BACKGROUND_PIXEL_SIZE);
}

/** @returns The pixels of a circle of that radius around (0, 0), from the
 * midpoint circle algorithm: one pixel thick, every pixel whole. */
std::vector<sf::Vector2i> circleOf(int radius) {
  std::vector<sf::Vector2i> pixels;
  int column = radius;
  int row = 0;
  int error = 1 - radius;
  while (column >= row) {
    for (const sf::Vector2i octant :
         {sf::Vector2i(column, row), sf::Vector2i(row, column),
          sf::Vector2i(-row, column), sf::Vector2i(-column, row),
          sf::Vector2i(-column, -row), sf::Vector2i(-row, -column),
          sf::Vector2i(row, -column), sf::Vector2i(column, -row)}) {
      pixels.push_back(octant);
    }
    ++row;
    if (error < 0) {
      error += (2 * row) + 1;
    } else {
      --column;
      error += (2 * (row - column)) + 1;
    }
  }
  return pixels;
}

void paintParticles(engine::ComponentRegistry& components,
                    IPixelSurface& surface) {
  components.forEach<Particle, game::Position>(
      [&surface](engine::Entity, const Particle& particle,
                 const game::Position& position) {
        if (particle.age < engine::Duration::zero() ||
            particle.colors.empty()) {
          return;
        }
        const float share = shareOfLife(particle.age, particle.life);
        const sf::Color color = colorAt(particle.colors, share);
        const sf::Vector2i pixel = pixelOf(position);
        const int size =
            particle.shrinks && share < PARTICLE_SHRINK_SHARE ? 2 : 1;
        for (int row = 0; row < size; ++row) {
          for (int column = 0; column < size; ++column) {
            surface.plot(pixel + sf::Vector2i(column, row), color);
          }
        }
      });
}

void paintShockwaves(engine::ComponentRegistry& components,
                     IPixelSurface& surface) {
  components.forEach<Shockwave, game::Position>(
      [&surface](engine::Entity, const Shockwave& shockwave,
                 const game::Position& position) {
        if (shockwave.age < engine::Duration::zero() ||
            shockwave.colors.empty()) {
          return;
        }
        const float share = shareOfLife(shockwave.age, shockwave.life);
        const int radius = std::max(
            1,
            static_cast<int>(std::lround(static_cast<float>(shockwave.radius) *
                                         easeOutCubic(share))));
        const sf::Color color = colorAt(shockwave.colors, share);
        const sf::Vector2i center = pixelOf(position);
        const bool dotted = share > 1.0F - SHOCKWAVE_DOTTED_SHARE;
        for (const sf::Vector2i offset : circleOf(radius)) {
          const sf::Vector2i pixel = center + offset;
          if (!dotted || (pixel.x + pixel.y) % 2 == 0) {
            surface.plot(pixel, color);
          }
        }
      });
}

void paintSpeedLines(engine::ComponentRegistry& components,
                     IPixelSurface& surface) {
  components.forEach<SpeedLine, game::Position>(
      [&surface](engine::Entity, const SpeedLine& line,
                 const game::Position& position) {
        if (line.age < engine::Duration::zero()) {
          return;
        }
        const float share = shareOfLife(line.age, line.life);
        const sf::Vector2i end = pixelOf(position);
        const auto length = static_cast<float>(end.x);
        const int head = static_cast<int>(
            std::lround(length * easeOutCubic(share * SPEED_LINE_HEAD_PACE)));
        const int tail = static_cast<int>(
            std::lround(length * easeOutCubic((share * SPEED_LINE_HEAD_PACE) -
                                              SPEED_LINE_TAIL_DELAY)));
        for (int column = tail; column < head; ++column) {
          surface.plot({column, end.y}, line.color);
        }
      });
}

}  // namespace

void paintGroundEffects(engine::ComponentRegistry& components,
                        IPixelSurface& surface) {
  paintShockwaves(components, surface);
  paintSpeedLines(components, surface);
  paintParticles(components, surface);
}

}  // namespace rtype::client

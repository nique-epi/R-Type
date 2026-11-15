#include "Starfield.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "BackgroundConstants.hpp"
#include "ColoredPixel.hpp"
#include "IPixelSurface.hpp"
#include "PixelRounding.hpp"
#include "RandomRange.hpp"
#include "Star.hpp"
#include "StarPlane.hpp"
#include "StarTwinkleStep.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @brief The pixel a star is painted on, and whether it twinkles there. */
struct StarPixel {
  sf::Vector2i pixel;
  bool twinkling{};
};

/** @returns How many pixels of streak a star at @p depth leaves behind at
 * @p speedFactor: what it covers in one SIMULATION_TICK_DURATION, none up to
 * STREAK_SPEED_FACTOR. */
int streakLength(StarDepth depth, float speedFactor) {
  if (speedFactor <= STREAK_SPEED_FACTOR) {
    return 0;
  }
  const float frameSeconds =
      std::chrono::duration<float>(engine::SIMULATION_TICK_DURATION).count();
  return static_cast<int>(
      std::lround(STAR_LAYERS[static_cast<std::size_t>(depth)].speed *
                  speedFactor * frameSeconds));
}

void paintStar(const Star& star, StarPixel where, IPixelSurface& surface) {
  switch (star.depth) {
    case StarDepth::Far:
      surface.plot(where.pixel, where.twinkling ? BLUE : DEEP_BLUE);
      return;
    case StarDepth::Middle:
      surface.plot(where.pixel, where.twinkling ? BLUE : PALE_BLUE);
      return;
    case StarDepth::Near:
      surface.plot(where.pixel, star.warm ? STAR_GOLD : STAR_WHITE);
      if (!where.twinkling) {
        for (const sf::Vector2i direction : CROSS_DIRECTIONS) {
          surface.plot(where.pixel + direction, PALE_BLUE);
        }
      }
      return;
  }
}

}  // namespace

Starfield::Starfield(std::uint32_t seed)
    : random_(seed), skyEvents_(static_cast<std::uint32_t>(random_())) {
  std::uniform_int_distribution<std::int64_t> twinklePhases(
      0, STAR_TWINKLE_CYCLE_STEPS - 1);
  for (const StarLayer& layer : STAR_LAYERS) {
    for (std::size_t i = 0; i < layer.count; ++i) {
      const sf::Vector2f position = randomPointBetween(
          random_, {0.0F, 0.0F}, {BACKGROUND_WIDTH, BACKGROUND_HEIGHT});
      const std::int64_t twinklePhase = twinklePhases(random_);
      stars_.push_back({.column = static_cast<int>(std::floor(position.x)),
                        .height = position.y,
                        .depth = layer.depth,
                        .twinklePhase = twinklePhase,
                        .warm = randomChance(random_, WARM_STAR_CHANCE)});
    }
  }
}

void Starfield::advance(engine::Duration elapsed) {
  const engine::Duration step =
      std::clamp(elapsed, engine::Duration::zero(), MAXIMUM_BACKGROUND_STEP);
  const float seconds = std::chrono::duration<float>(step).count();
  elapsed_ += step;
  for (const StarLayer& layer : STAR_LAYERS) {
    moveLayer(layer, layer.speed * seconds * speedFactor_);
  }
  planetLeft_ -= PLANET_SPEED * seconds;
  if (planetLeft_ < -static_cast<float>(PLANET_IMAGE_SIZE.x)) {
    planetLeft_ = BACKGROUND_WIDTH + PLANET_REENTRY_MARGIN;
  }
  skyEvents_.advance(seconds);
}

void Starfield::setSpeedFactor(float factor) {
  speedFactor_ = std::max(factor, 0.0F);
}

void Starfield::paint(StarPlane plane, IPixelSurface& surface) const {
  switch (plane) {
    case StarPlane::FarStars:
      paintStarsAt(StarDepth::Far, surface);
      return;
    case StarPlane::Planet:
      paintPlanet(surface);
      return;
    case StarPlane::MiddleStars:
      paintStarsAt(StarDepth::Middle, surface);
      return;
    case StarPlane::NearStars:
      paintStarsAt(StarDepth::Near, surface);
      skyEvents_.paint(surface);
      return;
  }
}

float Starfield::shiftOf(StarPlane plane) const {
  switch (plane) {
    case StarPlane::FarStars:
      return layerShifts_.at(static_cast<std::size_t>(StarDepth::Far));
    case StarPlane::Planet:
      return shiftPastPixel(planetLeft_);
    case StarPlane::MiddleStars:
      return layerShifts_.at(static_cast<std::size_t>(StarDepth::Middle));
    case StarPlane::NearStars:
      return 0.0F;
  }
  return 0.0F;
}

const std::vector<Star>& Starfield::stars() const { return stars_; }

float Starfield::positionOf(const Star& star) const {
  return static_cast<float>(star.column) +
         layerShifts_.at(static_cast<std::size_t>(star.depth));
}

float Starfield::planetLeft() const { return planetLeft_; }

void Starfield::moveLayer(const StarLayer& layer, float distance) {
  float& shift = layerShifts_.at(static_cast<std::size_t>(layer.depth));
  const float moved = shift - distance;
  shift = shiftPastPixel(moved);
  const auto columnsMoved = static_cast<int>(std::round(moved - shift));
  for (Star& star :
       std::ranges::equal_range(stars_, layer.depth, {}, &Star::depth)) {
    star.column += columnsMoved;
    if (star.column < -STAR_WRAP_MARGIN) {
      star.column += STAR_WRAP_DISTANCE;
      star.height = randomBetween(random_, 0.0F, BACKGROUND_HEIGHT);
    }
  }
}

void Starfield::paintPlanet(IPixelSurface& surface) const {
  const sf::Vector2i topLeft =
      nearestPixel({planetLeft_ - shiftOf(StarPlane::Planet),
                    BACKGROUND_HEIGHT * PLANET_TOP_SHARE_OF_HEIGHT});
  for (const ColoredPixel& pixel : planetImage_.pixels()) {
    surface.plot(topLeft + pixel.position, pixel.color);
  }
}

void Starfield::paintStarsAt(StarDepth depth, IPixelSurface& surface) const {
  const std::int64_t twinkleStep =
      std::chrono::duration_cast<StarTwinkleStep>(elapsed_).count();
  const int streak = streakLength(depth, speedFactor_);
  const sf::Color streakColor =
      STAR_STREAK_COLORS[static_cast<std::size_t>(depth)];
  for (const Star& star :
       std::ranges::equal_range(stars_, depth, {}, &Star::depth)) {
    const sf::Vector2i pixel(star.column,
                             static_cast<int>(std::lround(star.height)));
    for (int column = 1; column <= streak; ++column) {
      surface.plot(pixel + sf::Vector2i(column, 0), streakColor);
    }
    paintStar(star,
              StarPixel{.pixel = pixel,
                        .twinkling = (twinkleStep + star.twinklePhase) %
                                         STAR_TWINKLE_CYCLE_STEPS ==
                                     0},
              surface);
  }
}

}  // namespace rtype::client

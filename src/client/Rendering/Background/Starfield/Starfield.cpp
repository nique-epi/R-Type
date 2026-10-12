#include "Starfield.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <random>
#include <ranges>
#include <vector>
#include "BackgroundColors.hpp"
#include "BackgroundConstants.hpp"
#include "ColoredPixel.hpp"
#include "IPixelSurface.hpp"
#include "PixelRounding.hpp"
#include "RandomRange.hpp"
#include "Star.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

Starfield::Starfield(std::uint32_t seed)
    : random_(seed), skyEvents_(static_cast<std::uint32_t>(random_())) {
  std::uniform_int_distribution<std::int64_t> twinklePhases(
      0, STAR_TWINKLE_CYCLE_STEPS - 1);
  for (const StarLayer& layer : STAR_LAYERS) {
    for (std::size_t i = 0; i < layer.count; ++i) {
      const sf::Vector2f position = randomPointBetween(
          random_, {0.0F, 0.0F}, {BACKGROUND_WIDTH, BACKGROUND_HEIGHT});
      const std::int64_t twinklePhase = twinklePhases(random_);
      stars_.push_back({.position = position,
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
  for (Star& star : stars_) {
    moveStar(star, seconds);
  }
  planetLeft_ -= PLANET_SPEED * seconds;
  if (planetLeft_ < -static_cast<float>(PLANET_IMAGE_SIZE.x)) {
    planetLeft_ = BACKGROUND_WIDTH + PLANET_REENTRY_MARGIN;
  }
  skyEvents_.advance(seconds);
}

void Starfield::paint(IPixelSurface& surface) const {
  const std::int64_t twinkleStep =
      std::chrono::duration_cast<StarTwinkleStep>(elapsed_).count();
  const auto firstNearerStar =
      std::ranges::upper_bound(stars_, StarDepth::Far, {}, &Star::depth);
  for (const Star& star :
       std::ranges::subrange(stars_.begin(), firstNearerStar)) {
    paintStar(star, twinkleStep, surface);
  }
  paintPlanet(surface);
  for (const Star& star :
       std::ranges::subrange(firstNearerStar, stars_.end())) {
    paintStar(star, twinkleStep, surface);
  }
  skyEvents_.paint(surface);
}

const std::vector<Star>& Starfield::stars() const { return stars_; }

float Starfield::planetLeft() const { return planetLeft_; }

void Starfield::moveStar(Star& star, float seconds) {
  star.position.x -=
      STAR_LAYERS[static_cast<std::size_t>(star.depth)].speed * seconds;
  if (star.position.x < -STAR_WRAP_MARGIN) {
    star.position.x += STAR_WRAP_DISTANCE;
    star.position.y = randomBetween(random_, 0.0F, BACKGROUND_HEIGHT);
  }
}

void Starfield::paintPlanet(IPixelSurface& surface) const {
  const sf::Vector2i topLeft = nearestPixel(
      {planetLeft_, BACKGROUND_HEIGHT * PLANET_TOP_SHARE_OF_HEIGHT});
  for (const ColoredPixel& pixel : planetImage_.pixels()) {
    surface.plot(topLeft + pixel.position, pixel.color);
  }
}

void Starfield::paintStar(const Star& star, std::int64_t twinkleStep,
                          IPixelSurface& surface) {
  const sf::Vector2i pixel = nearestPixel(star.position);
  const bool twinkling =
      (twinkleStep + star.twinklePhase) % STAR_TWINKLE_CYCLE_STEPS == 0;
  switch (star.depth) {
    case StarDepth::Far:
      surface.plot(pixel, twinkling ? BLUE : DEEP_BLUE);
      return;
    case StarDepth::Middle:
      surface.plot(pixel, twinkling ? BLUE : PALE_BLUE);
      return;
    case StarDepth::Near:
      surface.plot(pixel, star.warm ? STAR_GOLD : STAR_WHITE);
      if (!twinkling) {
        for (const sf::Vector2i direction : CROSS_DIRECTIONS) {
          surface.plot(pixel + direction, PALE_BLUE);
        }
      }
      return;
  }
}

}  // namespace rtype::client

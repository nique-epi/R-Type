#include "StarfieldFixtures.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <vector>
#include "BackgroundConstants.hpp"
#include "BackgroundTestConstants.hpp"
#include "RecordingPixelSurface.hpp"
#include "Star.hpp"
#include "StarPlane.hpp"
#include "Starfield.hpp"
#include "StarfieldTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::MAXIMUM_BACKGROUND_STEP;
using rtype::client::Star;
using rtype::client::STAR_LAYERS;
using rtype::client::STAR_PLANES;
using rtype::client::STAR_WRAP_DISTANCE;
using rtype::client::StarDepth;
using rtype::client::Starfield;
using rtype::client::StarPlane;
using rtype::engine::Duration;

float seconds(Duration duration) {
  return std::chrono::duration<float>(duration).count();
}

void advanceBy(Starfield& starfield, Duration total) {
  for (Duration left = total; left > Duration::zero();
       left -= MAXIMUM_BACKGROUND_STEP) {
    starfield.advance(std::min(left, MAXIMUM_BACKGROUND_STEP));
  }
}

float speedOf(StarDepth depth) {
  return STAR_LAYERS[static_cast<std::size_t>(depth)].speed;
}

std::size_t countAt(const Starfield& starfield, StarDepth depth) {
  return static_cast<std::size_t>(
      std::ranges::count(starfield.stars(), depth, &Star::depth));
}

void expectSameColumns(const Starfield& left, const Starfield& right) {
  ASSERT_EQ(left.stars().size(), right.stars().size());
  for (std::size_t i = 0; i < left.stars().size(); ++i) {
    const float apart = std::fabs(left.positionOf(left.stars()[i]) -
                                  right.positionOf(right.stars()[i]));
    const auto wrap = static_cast<float>(STAR_WRAP_DISTANCE);
    EXPECT_NEAR(std::min(apart, wrap - apart), 0.0F, POSITION_TOLERANCE);
  }
}

std::size_t plotCountAt(float speedFactor) {
  Starfield starfield(SEED);
  starfield.setSpeedFactor(speedFactor);
  RecordingPixelSurface surface;
  for (const StarPlane plane : STAR_PLANES) {
    starfield.paint(plane, surface);
  }
  return surface.plots().size();
}

std::vector<float> positionsOf(const Starfield& starfield) {
  std::vector<float> positions;
  for (const Star& star : starfield.stars()) {
    positions.push_back(starfield.positionOf(star));
  }
  return positions;
}

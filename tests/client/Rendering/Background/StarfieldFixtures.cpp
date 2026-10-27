#include "StarfieldFixtures.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include "BackgroundConstants.hpp"
#include "BackgroundTestConstants.hpp"
#include "RecordingPixelSurface.hpp"
#include "SkyLight.hpp"
#include "Star.hpp"
#include "Starfield.hpp"
#include "StarfieldTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::MAXIMUM_BACKGROUND_STEP;
using rtype::client::SkyLight;
using rtype::client::Star;
using rtype::client::STAR_LAYERS;
using rtype::client::StarDepth;
using rtype::client::Starfield;
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
    EXPECT_NEAR(left.stars()[i].position.x, right.stars()[i].position.x,
                POSITION_TOLERANCE);
  }
}

std::size_t plotCountAt(float speedFactor) {
  Starfield starfield(SEED);
  starfield.setSpeedFactor(speedFactor);
  RecordingPixelSurface surface;
  starfield.paint(surface, SkyLight::Twinkling);
  return surface.plots().size();
}

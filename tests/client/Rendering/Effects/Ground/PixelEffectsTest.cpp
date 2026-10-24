#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <array>
#include <cmath>
#include <cstddef>
#include <numbers>
#include <utility>
#include <vector>
#include "EffectTestConstants.hpp"
#include "Particle.hpp"
#include "PixelEffects.hpp"
#include "PixelEffectsFixtures.hpp"
#include "PixelEffectsTestConstants.hpp"
#include "PixelScene.hpp"
#include "Position.hpp"
#include "Shockwave.hpp"
#include "SpriteToneColorConstants.hpp"

using rtype::client::FLASH_COLOR;
using rtype::client::Particle;
using rtype::client::Shockwave;
using ::testing::ElementsAre;

/**
 * Given no particle alive
 * When a burst of 8 drops waits 50 ms
 * Then 8 particles are aged minus 50 ms: hidden and still until then
 */
TEST(PixelEffects, ABurstWaitsWithANegativeAge) {
  PixelScene scene;

  scene.pixels().burst(eightDropsAimedLeft());

  const std::vector<Particle> particles = scene.particles();
  ASSERT_EQ(particles.size(), EIGHT_DROPS.count);
  for (const Particle& particle : particles) {
    EXPECT_EQ(particle.age, -WAIT);
  }
}

/**
 * Given 184 particles alive
 * When two bursts of 8 come
 * Then only the first is added whole: never more than 192 alive
 */
TEST(PixelEffects, NeverMoreThanTheMostParticlesAlive) {
  PixelScene scene;
  for (std::size_t burst = 0; burst < (MOST_PARTICLES / EIGHT_DROPS.count) - 1;
       ++burst) {
    scene.pixels().burst(eightDropsAimedLeft());
  }

  scene.pixels().burst(eightDropsAimedLeft());
  scene.pixels().burst(eightDropsAimedLeft());

  EXPECT_EQ(scene.particles().size(), MOST_PARTICLES);
}

/**
 * Given a burst aimed left in a cone of one radian
 * When it bursts
 * Then every particle leaves within half a radian of the left, between its
 * slowest and its fastest speeds
 */
TEST(PixelEffects, ParticlesLeaveInTheirConeAtTheirSpeed) {
  PixelScene scene;

  scene.pixels().burst(eightDropsAimedLeft());

  for (const Particle& particle : scene.particles()) {
    const float speed = std::hypot(particle.velocity.x, particle.velocity.y);
    const float fromAim = std::remainder(
        std::atan2(particle.velocity.y, particle.velocity.x) - LEFT,
        2.0F * std::numbers::pi_v<float>);
    EXPECT_GE(speed, EIGHT_DROPS.slowest - BURST_TOLERANCE);
    EXPECT_LE(speed, EIGHT_DROPS.fastest + BURST_TOLERANCE);
    EXPECT_LE(std::abs(fromAim), (EIGHT_DROPS.cone / 2.0F) + BURST_TOLERANCE);
  }
}

/**
 * Given nothing alive
 * When a shockwave of 24 pixels waits 50 ms
 * Then one shockwave of that radius is aged minus 50 ms
 */
TEST(PixelEffects, AWaveIsOneShockwaveThatWaits) {
  PixelScene scene;

  scene.pixels().wave(
      {.shape = WIDE_RING, .center = CENTER, .colors = RAMP, .delay = WAIT});

  const std::vector<Shockwave> waves = scene.shockwaves();
  ASSERT_EQ(waves.size(), 1U);
  EXPECT_EQ(waves.front().radius, WIDE_RING.radius);
  EXPECT_EQ(waves.front().age, -WAIT);
}

/**
 * Given a ship arriving, with flashes
 * When its speed lines are laid
 * Then four lines, 6 and 2 pixels above and under it, white and in the light
 * of its pilot in turn
 */
TEST(PixelEffects, AnArrivalLaysFourLinesWhiteAndLightInTurn) {
  PixelScene scene;

  scene.pixels().speedLines({.shipCenter = CENTER,
                             .light = PILOT_LIGHT,
                             .flashes = true,
                             .delay = WAIT});

  using Line = std::pair<float, sf::Color>;
  EXPECT_THAT(scene.speedLines(),
              ElementsAre(Line{CENTER.y + SPEED_LINE_OFFSETS[0], FLASH_COLOR},
                          Line{CENTER.y + SPEED_LINE_OFFSETS[1], PILOT_LIGHT},
                          Line{CENTER.y + SPEED_LINE_OFFSETS[2], FLASH_COLOR},
                          Line{CENTER.y + SPEED_LINE_OFFSETS[3], PILOT_LIGHT}));
}

/**
 * Given a ship arriving, with flashes reduced
 * When its speed lines are laid
 * Then no line is white
 */
TEST(PixelEffects, AnArrivalWithoutFlashLaysNoWhiteLine) {
  PixelScene scene;

  scene.pixels().speedLines({.shipCenter = CENTER,
                             .light = PILOT_LIGHT,
                             .flashes = false,
                             .delay = WAIT});

  for (const auto& [row, color] : scene.speedLines()) {
    EXPECT_EQ(color, PILOT_LIGHT);
  }
}

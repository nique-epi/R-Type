#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <vector>
#include "ColoredPixel.hpp"
#include "EffectTestConstants.hpp"
#include "GroundEffectsFixture.hpp"
#include "GroundEffectsTestConstants.hpp"
#include "Shockwave.hpp"
#include "SpeedLine.hpp"

using rtype::client::ColoredPixel;
using rtype::client::Shockwave;
using rtype::client::SpeedLine;

/**
 * Given a particle still waiting, aged minus 10 ms
 * When the ground effects are painted
 * Then no pixel is painted
 */
TEST_F(GroundEffectsFixture, AWaitingParticleIsNotPainted) {
  addEffect(ON_PIXEL_100_50, shrinkingDropAged(-STILL_WAITING));

  EXPECT_TRUE(paint().empty());
}

/**
 * Given a drop that shrinks, at a quarter of its life
 * When the ground effects are painted
 * Then it is two by two pixels, from its own pixel down and right
 */
TEST_F(GroundEffectsFixture, AYoungDropIsTwoByTwoPixels) {
  addEffect(ON_PIXEL_100_50, shrinkingDropAged(YOUNG));

  const std::vector<ColoredPixel> plots = paint();

  ASSERT_EQ(plots.size(), 4U);
  for (const ColoredPixel& plot : plots) {
    EXPECT_GE(plot.position.x, PIXEL_100_50.x);
    EXPECT_LE(plot.position.x, PIXEL_100_50.x + 1);
    EXPECT_GE(plot.position.y, PIXEL_100_50.y);
    EXPECT_LE(plot.position.y, PIXEL_100_50.y + 1);
  }
}

/**
 * Given a drop that shrinks, past two fifths of its life
 * When the ground effects are painted
 * Then it is one pixel
 */
TEST_F(GroundEffectsFixture, AnOldDropIsOnePixel) {
  addEffect(ON_PIXEL_100_50, shrinkingDropAged(PAST_TWO_FIFTHS));

  EXPECT_EQ(paint().size(), 1U);
}

/**
 * Given a shockwave of 24 pixels at half its life
 * When the ground effects are painted
 * Then every pixel lies about 21 pixels from its center: its radius opens
 * fast, then slowly (7/8 of the way at half its life)
 */
TEST_F(GroundEffectsFixture, AShockwaveOpensFastThenSlowly) {
  addEffect(ON_PIXEL_100_50, Shockwave{.radius = RING_RADIUS,
                                       .age = A_PARTICLE_LIFE / 2,
                                       .life = A_PARTICLE_LIFE,
                                       .colors = RAMP});

  const std::vector<ColoredPixel> plots = paint();

  ASSERT_FALSE(plots.empty());
  for (const ColoredPixel& plot : plots) {
    const sf::Vector2i fromCenter = plot.position - PIXEL_100_50;
    const float distance = std::hypot(static_cast<float>(fromCenter.x),
                                      static_cast<float>(fromCenter.y));
    EXPECT_NEAR(distance, static_cast<float>(RADIUS_AT_HALF_LIFE), 1.0F);
  }
}

/**
 * Given a shockwave in the last tenth of its life
 * When the ground effects are painted
 * Then it lights only pixels whose column plus row is even
 */
TEST_F(GroundEffectsFixture, AShockwaveEndsDotted) {
  addEffect(ON_PIXEL_100_50, Shockwave{.radius = RING_RADIUS,
                                       .age = LAST_TENTH,
                                       .life = A_PARTICLE_LIFE,
                                       .colors = RAMP});

  const std::vector<ColoredPixel> plots = paint();

  ASSERT_FALSE(plots.empty());
  for (const ColoredPixel& plot : plots) {
    EXPECT_EQ((plot.position.x + plot.position.y) % 2, 0);
  }
}

/**
 * Given a speed line ending at column 100, at a quarter of its life
 * When the ground effects are painted
 * Then it runs on its row from column 0, its tail not yet left, to its head,
 * which ran most of the way
 */
TEST_F(GroundEffectsFixture, ASpeedLineLeavesFromTheLeftEdge) {
  addEffect(ON_PIXEL_100_50, SpeedLine{.age = A_PARTICLE_LIFE / 4,
                                       .life = A_PARTICLE_LIFE,
                                       .color = sf::Color::White});

  const std::vector<ColoredPixel> plots = paint();

  ASSERT_FALSE(plots.empty());
  EXPECT_EQ(plots.front().position, sf::Vector2i(0, PIXEL_100_50.y));
  for (const ColoredPixel& plot : plots) {
    EXPECT_EQ(plot.position.y, PIXEL_100_50.y);
    EXPECT_LT(plot.position.x, PIXEL_100_50.x);
  }
}

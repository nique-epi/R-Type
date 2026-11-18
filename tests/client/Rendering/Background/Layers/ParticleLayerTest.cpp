#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstddef>
#include <vector>
#include "ColoredPixel.hpp"
#include "LayerFrame.hpp"
#include "LayerParticle.hpp"
#include "LayerTestConstants.hpp"
#include "ParticleLayer.hpp"
#include "ParticleLayerFixtures.hpp"
#include "ParticleLayerTestConstants.hpp"
#include "RecordedFrame.hpp"

using rtype::client::ColoredPixel;
using rtype::client::LayerFrame;
using rtype::client::LayerParticle;
using rtype::client::ParticleLayer;

/**
 * Given the far stars, a population of 441
 * When their layer is built
 * Then it holds 441 particles before any time passes
 */
TEST(ParticleLayer, HoldsItsPopulationFromTheStart) {
  const ParticleLayer stars = particleLayerOf(THE_FAR_STARS, STAR_REPEAT);

  EXPECT_EQ(stars.particles().size(), FAR_STARS_IN_THE_SKY);
  EXPECT_EQ(stars.aliveCount(), FAR_STARS_IN_THE_SKY);
}

/**
 * Given the far stars, born anywhere on the background
 * When their layer is built
 * Then each lies on a whole column of the background, at one of its heights
 */
TEST(ParticleLayer, SettlesItsPopulationInItsAreaOnWholeColumns) {
  const ParticleLayer stars = particleLayerOf(THE_FAR_STARS, STAR_REPEAT);

  for (const LayerParticle& star : stars.particles()) {
    EXPECT_GE(star.position.x, 0.0F);
    EXPECT_LT(star.position.x, WIDTH_OF_THE_BACKGROUND);
    EXPECT_FLOAT_EQ(star.position.x, std::floor(star.position.x));
    EXPECT_GE(star.position.y, 0.0F);
    EXPECT_LE(star.position.y, HEIGHT_OF_THE_BACKGROUND);
  }
}

/**
 * Given one deep blue star at (100, 50) that leaves no streak
 * When it is painted, its layer at its origin
 * Then one pixel is plotted, there, in deep blue
 */
TEST(ParticleLayer, PaintsAPixelParticleInItsColor) {
  const ParticleLayer star =
      particleLayerOf(ONE_STAR_WITHOUT_A_STREAK, STAR_REPEAT);

  const std::vector<ColoredPixel> pixels = pixelsPaintedBy(star, AT_ITS_ORIGIN);

  ASSERT_EQ(pixels.size(), 1U);
  EXPECT_EQ(pixels.front().position, WHERE_THE_STAR_LIES);
  EXPECT_EQ(pixels.front().color, DEEP_BLUE_STAR);
}

/**
 * Given one star at (100, 50) of its layer
 * When ten seconds pass, the layer 10 columns to the left
 * Then the star has not moved in its layer, and lies 10 columns left on the
 * background
 */
TEST(ParticleLayer, APopulationOnlyMovesWithItsLayer) {
  ParticleLayer star = particleLayerOf(ONE_STAR, STAR_REPEAT);

  star.advance(TEN_SECONDS_OF_LAYER, TEN_COLUMNS_LEFT);

  ASSERT_EQ(star.particles().size(), 1U);
  EXPECT_EQ(star.particles().front().position, PLACE_OF_THE_STAR);
  EXPECT_EQ(star.placeOf(star.particles().front(), TEN_COLUMNS_LEFT.column),
            STAR_TEN_COLUMNS_LEFT);
}

/**
 * Given one star that twinkles in blue one step in eight
 * When it is painted at each of eight twinkle steps in a row
 * Then it is blue at exactly one of them
 */
TEST(ParticleLayer, APixelTwinklesOneStepInEight) {
  ParticleLayer twinkling = particleLayerOf(ONE_STAR, STAR_REPEAT);
  int twinkles = 0;

  for (int step = 0; step < STEPS_OF_A_TWINKLE_CYCLE; ++step) {
    const std::vector<ColoredPixel> pixels =
        pixelsPaintedBy(twinkling, AT_ITS_ORIGIN);
    ASSERT_EQ(pixels.size(), 1U);
    twinkles += pixels.front().color == BLUE_TWINKLE ? 1 : 0;
    twinkling.advance(JUST_OVER_A_TWINKLE_STEP, AT_ITS_ORIGIN);
  }

  EXPECT_EQ(twinkles, 1);
}

/**
 * Given one near star drawn with a sprite of two frames, that twinkles one
 * step in eight
 * When it is painted at each of eight twinkle steps in a row
 * Then it shows its second frame at exactly one of them, its first at the
 * others, its middle on the star
 */
TEST(ParticleLayer, ASpriteShowsItsSecondFrameWhileItTwinkles) {
  ParticleLayer star = particleLayerOf(ONE_NEAR_STAR, STAR_REPEAT);
  int twinkles = 0;
  int crosses = 0;

  for (int step = 0; step < STEPS_OF_A_TWINKLE_CYCLE; ++step) {
    const std::vector<RecordedFrame> frames =
        spritesPaintedBy(star, AT_ITS_ORIGIN);
    ASSERT_EQ(frames.size(), 1U);
    EXPECT_EQ(frames.front().topLeft, CORNER_OF_THE_NEAR_STAR);
    twinkles += frames.front().frame == FRAME_WHILE_IT_TWINKLES ? 1 : 0;
    crosses += frames.front().frame == FRAME_WITH_ITS_CROSS ? 1 : 0;
    star.advance(JUST_OVER_A_TWINKLE_STEP, AT_ITS_ORIGIN);
  }

  EXPECT_EQ(twinkles, 1);
  EXPECT_EQ(crosses, STEPS_OF_A_TWINKLE_CYCLE - 1);
}

/**
 * Given one star at (100, 50) whose layer warps, a streak of 3 pixels
 * When it is painted
 * Then the three pixels right of it are plotted in the warp color, before
 * the star itself
 */
TEST(ParticleLayer, PaintsTheStreakOfAWarpRightOfTheParticle) {
  const ParticleLayer star = particleLayerOf(ONE_STAR, STAR_REPEAT);

  const std::vector<ColoredPixel> pixels = pixelsPaintedBy(star, IN_A_WARP);

  ASSERT_EQ(pixels.size(), static_cast<std::size_t>(STREAK_OF_THE_WARP) + 1);
  for (int column = 1; column <= STREAK_OF_THE_WARP; ++column) {
    const ColoredPixel& streak = pixels[static_cast<std::size_t>(column) - 1];
    EXPECT_EQ(streak.position, WHERE_THE_STAR_LIES + sf::Vector2i(column, 0));
    EXPECT_EQ(streak.color, PALE_BLUE_STREAK);
  }
  EXPECT_EQ(pixels.back().position, WHERE_THE_STAR_LIES);
}

/**
 * Given one star that gives no warp color, whose layer warps
 * When it is painted
 * Then the star alone is plotted
 */
TEST(ParticleLayer, LeavesNoStreakWithoutAWarpColor) {
  const ParticleLayer star =
      particleLayerOf(ONE_STAR_WITHOUT_A_STREAK, STAR_REPEAT);

  EXPECT_EQ(pixelsPaintedBy(star, IN_A_WARP).size(), 1U);
}

/**
 * Given the far stars in a layer that repeats every 488 columns
 * When the layer goes 2000 columns to the left, a column at a time
 * Then every star stays from 4 columns left of the background to 4 columns
 * right of it
 */
TEST(ParticleLayer, KeepsAPopulationWithinTheBandOfItsRepeat) {
  ParticleLayer stars = particleLayerOf(THE_FAR_STARS, STAR_REPEAT);

  for (int moved = 1; moved <= COLUMNS_OF_A_LONG_FLIGHT; ++moved) {
    stars.advance(ONE_FRAME_OF_LAYER, LayerFrame{.column = -moved});
    for (const LayerParticle& star : stars.particles()) {
      const float column = stars.placeOf(star, -moved).x;
      ASSERT_GE(column, -HIDDEN_ON_EACH_SIDE);
      ASSERT_LT(column, WIDTH_OF_THE_BACKGROUND + HIDDEN_ON_EACH_SIDE);
    }
  }
}

/**
 * Given one star that may lie at any height, in a layer that repeats
 * When the layer goes 2000 columns to the left, a column at a time
 * Then each time the star comes back from the right, it is at a new height
 */
TEST(ParticleLayer, AParticleThatComesBackTakesANewHeight) {
  ParticleLayer star = particleLayerOf(ONE_STAR_OF_ANY_HEIGHT, STAR_REPEAT);
  ASSERT_EQ(star.particles().size(), 1U);
  sf::Vector2f before = star.placeOf(star.particles().front(), 0);
  int returns = 0;

  for (int moved = 1; moved <= COLUMNS_OF_A_LONG_FLIGHT; ++moved) {
    star.advance(ONE_FRAME_OF_LAYER, LayerFrame{.column = -moved});
    const sf::Vector2f now = star.placeOf(star.particles().front(), -moved);
    if (now.x > before.x) {
      ++returns;
      EXPECT_NE(now.y, before.y);
    } else {
      EXPECT_FLOAT_EQ(now.y, before.y);
    }
    before = now;
  }

  EXPECT_GT(returns, 0);
}

/**
 * Given one star in a layer that never repeats
 * When the layer lies a thousand columns to the left
 * Then the star lies a thousand columns left of its place, out of sight
 */
TEST(ParticleLayer, WithoutARepeatAParticleLeavesWithItsLayer) {
  const ParticleLayer star = particleLayerOf(ONE_STAR, NO_PARTICLE_REPEAT);

  ASSERT_EQ(star.particles().size(), 1U);
  EXPECT_EQ(
      star.placeOf(star.particles().front(), A_THOUSAND_COLUMNS_LEFT.column),
      STAR_A_THOUSAND_COLUMNS_LEFT);
}

/**
 * Given a thousand stars drawn with two sprites, the first four times as
 * likely as the second
 * When their layer is built
 * Then about four in five are drawn with the first
 */
TEST(ParticleLayer, DrawsTheSpritesOfItsParticlesByTheirWeights) {
  const ParticleLayer stars =
      particleLayerOf(A_THOUSAND_STARS_OF_TWO_SPRITES, STAR_REPEAT);

  const int withTheFirst = particlesDrawnWith(stars, FIRST_SPRITE);

  EXPECT_GE(withTheFirst, FEWEST_OF_THE_FIRST_SPRITE);
  EXPECT_LE(withTheFirst, MOST_OF_THE_FIRST_SPRITE);
}

/**
 * Given the particles of a group
 * When their layer is built, and time passes without a birth
 * Then it holds no particle
 */
TEST(ParticleLayer, HoldsNoParticleOfAGroupBeforeABirth) {
  ParticleLayer events =
      particleLayerOf(LEFTWARD_PARTICLES, NO_PARTICLE_REPEAT);

  events.advance(ONE_SECOND_OF_LAYER, AT_ITS_ORIGIN);

  EXPECT_EQ(events.aliveCount(), 0U);
}

/**
 * Given particles born in a box of 100 by 50 pixels at (300, 20)
 * When a hundred are born
 * Then each lies in that box
 */
TEST(ParticleLayer, AParticleIsBornInItsArea) {
  ParticleLayer events =
      particleLayerOf(PARTICLES_BORN_IN_A_BOX, NO_PARTICLE_REPEAT);

  for (std::size_t birth = 0; birth < A_HUNDRED_BIRTHS; ++birth) {
    events.emit();
  }

  ASSERT_EQ(events.aliveCount(), A_HUNDRED_BIRTHS);
  for (const LayerParticle& particle : events.particles()) {
    EXPECT_GE(particle.position.x, LEFT_OF_THE_BOX);
    EXPECT_LE(particle.position.x, RIGHT_OF_THE_BOX);
    EXPECT_GE(particle.position.y, TOP_OF_THE_BOX);
    EXPECT_LE(particle.position.y, BOTTOM_OF_THE_BOX);
  }
}

/**
 * Given a particle born at (200, 100) that flies at 60 pixels per second and
 * gives no angle
 * When one second passes
 * Then it lies 60 pixels to the left, at the same height
 */
TEST(ParticleLayer, AParticleFliesToTheLeftAtItsSpeed) {
  ParticleLayer events =
      particleLayerOf(LEFTWARD_PARTICLES, NO_PARTICLE_REPEAT);
  events.emit();

  events.advance(ONE_SECOND_OF_LAYER, AT_ITS_ORIGIN);

  ASSERT_EQ(events.aliveCount(), 1U);
  const sf::Vector2f place = events.particles().front().position;
  EXPECT_NEAR(place.x, BIRTH_POINT.x - SIXTY_PIXELS, ROUNDING_OF_A_FLIGHT);
  EXPECT_NEAR(place.y, BIRTH_POINT.y, ROUNDING_OF_A_FLIGHT);
}

/**
 * Given a particle born at (200, 100) that flies at 60 pixels per second, at
 * an angle of 90 degrees
 * When one second passes
 * Then it lies 60 pixels lower, on the same column
 */
TEST(ParticleLayer, AnAngleOfNinetyDegreesFliesDown) {
  ParticleLayer events =
      particleLayerOf(DOWNWARD_PARTICLES, NO_PARTICLE_REPEAT);
  events.emit();

  events.advance(ONE_SECOND_OF_LAYER, AT_ITS_ORIGIN);

  ASSERT_EQ(events.aliveCount(), 1U);
  const sf::Vector2f place = events.particles().front().position;
  EXPECT_NEAR(place.x, BIRTH_POINT.x, ROUNDING_OF_A_FLIGHT);
  EXPECT_NEAR(place.y, BIRTH_POINT.y + SIXTY_PIXELS, ROUNDING_OF_A_FLIGHT);
}

/**
 * Given a hundred particles that drift by at most 9 pixels per second
 * When one second passes
 * Then none is more than 9 pixels above or below where it was born, and they
 * are not all at the same height
 */
TEST(ParticleLayer, AParticleDriftsByAtMostItsDrift) {
  ParticleLayer events =
      particleLayerOf(DRIFTING_PARTICLES, NO_PARTICLE_REPEAT);
  for (std::size_t birth = 0; birth < A_HUNDRED_BIRTHS; ++birth) {
    events.emit();
  }

  events.advance(ONE_SECOND_OF_LAYER, AT_ITS_ORIGIN);

  ASSERT_EQ(events.aliveCount(), A_HUNDRED_BIRTHS);
  bool apart = false;
  for (const LayerParticle& particle : events.particles()) {
    const float drifted = particle.position.y - BIRTH_POINT.y;
    EXPECT_LE(std::abs(drifted), MOST_DRIFT + ROUNDING_OF_A_FLIGHT);
    apart =
        apart || particle.position.y != events.particles().front().position.y;
  }
  EXPECT_TRUE(apart);
}

/**
 * Given a flare that lives one second
 * When 990 ms pass, then 20 more
 * Then it lives after the first, and has ended after the second
 */
TEST(ParticleLayer, AParticleEndsWhenItsLifeIsOver) {
  ParticleLayer flares =
      particleLayerOf(FLARES_OF_ONE_SECOND, NO_PARTICLE_REPEAT);
  flares.emit();

  flares.advance(JUST_SHORT_OF_A_SECOND, AT_ITS_ORIGIN);
  const std::size_t beforeItsEnd = flares.aliveCount();
  flares.advance(PAST_A_SECOND, AT_ITS_ORIGIN);

  EXPECT_EQ(beforeItsEnd, 1U);
  EXPECT_EQ(flares.aliveCount(), 0U);
}

/**
 * Given a shooting star that lives 100 ms, whose trail takes 80 ms more to
 * shrink
 * When 150 ms pass, a frame of 10 ms at a time, then 50 more
 * Then it still shows after the first, its trail shrinking, and has ended
 * after the second
 */
TEST(ParticleLayer, AParticleWithATrailEndsOnceItsTrailHasShrunk) {
  ParticleLayer stars =
      particleLayerOf(SHOOTING_STARS_OF_A_TENTH, NO_PARTICLE_REPEAT);
  stars.emit();

  for (int frame = 0; frame < FRAMES_UNTIL_ITS_TRAIL_SHRINKS; ++frame) {
    stars.advance(TEN_MILLISECONDS, AT_ITS_ORIGIN);
  }
  const std::size_t whileItShrinks = stars.aliveCount();
  for (int frame = 0; frame < FRAMES_UNTIL_ITS_TRAIL_HAS_SHRUNK; ++frame) {
    stars.advance(TEN_MILLISECONDS, AT_ITS_ORIGIN);
  }

  EXPECT_EQ(whileItShrinks, 1U);
  EXPECT_EQ(stars.aliveCount(), 0U);
}

/**
 * Given an asteroid without a life, born 6 pixels right of the background,
 * that ends 6 pixels left of it, 492 pixels flown at 100 per second
 * When 4.9 s pass, then 50 ms more
 * Then it lives after the first, 2 pixels short, and has ended after the
 * second
 */
TEST(ParticleLayer, AParticleWithoutALifeEndsOnceOutOfTheBackground) {
  ParticleLayer asteroids =
      particleLayerOf(ASTEROIDS_ACROSS_THE_SKY, NO_PARTICLE_REPEAT);
  asteroids.emit();

  asteroids.advance(JUST_SHORT_OF_THE_CROSSING, AT_ITS_ORIGIN);
  const std::size_t beforeItLeaves = asteroids.aliveCount();
  asteroids.advance(PAST_THE_CROSSING, AT_ITS_ORIGIN);

  EXPECT_EQ(beforeItLeaves, 1U);
  EXPECT_EQ(asteroids.aliveCount(), 0U);
}

/**
 * Given a flare of four frames that lives one second, born at (200, 100)
 * When it is painted 100 ms in, then 600 ms in
 * Then it shows its first frame, then its third, its middle on the flare
 */
TEST(ParticleLayer, ASpriteWithoutARateShowsItsFramesOverTheLife) {
  ParticleLayer flares =
      particleLayerOf(FLARES_OF_ONE_SECOND, NO_PARTICLE_REPEAT);
  flares.emit();

  flares.advance(A_TENTH_OF_A_SECOND, AT_ITS_ORIGIN);
  const std::vector<RecordedFrame> early =
      spritesPaintedBy(flares, AT_ITS_ORIGIN);
  flares.advance(HALF_A_SECOND, AT_ITS_ORIGIN);
  const std::vector<RecordedFrame> later =
      spritesPaintedBy(flares, AT_ITS_ORIGIN);

  ASSERT_EQ(early.size(), 1U);
  ASSERT_EQ(later.size(), 1U);
  EXPECT_EQ(early.front().frame, FIRST_FRAME_OF_THE_FLARE);
  EXPECT_EQ(early.front().topLeft, CORNER_OF_THE_FLARE);
  EXPECT_EQ(later.front().frame, THIRD_FRAME_OF_THE_FLARE);
}

/**
 * Given an asteroid whose four frames loop at 4 frames per second
 * When a quarter of a second passes
 * Then it shows the frame after the one it showed, the first after the last
 */
TEST(ParticleLayer, ASpriteThatLoopsMovesOnAtItsFrameRate) {
  ParticleLayer asteroids =
      particleLayerOf(ASTEROIDS_ACROSS_THE_SKY, NO_PARTICLE_REPEAT);
  asteroids.emit();
  const std::vector<RecordedFrame> before =
      spritesPaintedBy(asteroids, AT_ITS_ORIGIN);

  asteroids.advance(ONE_FRAME_OF_THE_SPIN, AT_ITS_ORIGIN);
  const std::vector<RecordedFrame> after =
      spritesPaintedBy(asteroids, AT_ITS_ORIGIN);

  ASSERT_EQ(before.size(), 1U);
  ASSERT_EQ(after.size(), 1U);
  EXPECT_EQ(after.front().frame,
            (before.front().frame + 1) % FRAMES_OF_THE_ASTEROID);
}

/**
 * Given a shooting star that flies left, 10 ms after its birth at
 * (200, 100), with a trail of 4 pixels, white up to its half then deep blue
 * When it is painted
 * Then three pixels of trail lie right of it, the first white, the last
 * deep blue, then the star itself, white, on (199, 100)
 */
TEST(ParticleLayer, PaintsItsTrailBehindItInTheColorsOfItsStops) {
  ParticleLayer stars =
      particleLayerOf(SHOOTING_STARS_WITH_A_SHORT_TRAIL, NO_PARTICLE_REPEAT);
  stars.emit();
  stars.advance(TEN_MILLISECONDS, AT_ITS_ORIGIN);

  const std::vector<ColoredPixel> pixels =
      pixelsPaintedBy(stars, AT_ITS_ORIGIN);

  ASSERT_EQ(pixels.size(), PIXELS_OF_THE_SHORT_TRAIL);
  EXPECT_EQ(pixels.front().position, FIRST_PIXEL_OF_THE_TRAIL);
  EXPECT_EQ(pixels.front().color, WHITE_STAR);
  EXPECT_EQ(pixels[LAST_OF_THE_TRAIL].position, LAST_PIXEL_OF_THE_TRAIL);
  EXPECT_EQ(pixels[LAST_OF_THE_TRAIL].color, DEEP_BLUE_STAR);
  EXPECT_EQ(pixels.back().position, HEAD_OF_THE_SHOOTING_STAR);
  EXPECT_EQ(pixels.back().color, WHITE_STAR);
}

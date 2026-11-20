#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <chrono>
#include <string>
#include "BackgroundDescription.hpp"
#include "BackgroundReadingFixtures.hpp"
#include "BackgroundReadingTestConstants.hpp"
#include "ClientException.hpp"
#include "DecimalRange.hpp"
#include "DurationRange.hpp"
#include "EmissionGroupDescription.hpp"
#include "EngineException.hpp"
#include "ImageContent.hpp"
#include "LayerDescription.hpp"
#include "LayerMotion.hpp"
#include "MapContent.hpp"
#include "MapLayerKind.hpp"
#include "ParticleContent.hpp"
#include "ParticleSprite.hpp"
#include "ParticleTrail.hpp"

using rtype::client::BackgroundDescription;
using rtype::client::DurationRange;
using rtype::client::EmissionGroupDescription;
using rtype::client::ImageContent;
using rtype::client::InconsistentBackgroundException;
using rtype::client::LayerDescription;
using rtype::client::LayerMotion;
using rtype::client::MapContent;
using rtype::client::MapLayerKind;
using rtype::client::ParticleContent;
using rtype::client::ParticleSprite;
using rtype::client::ParticleTrail;
using rtype::client::TooManyBackgroundTablesException;
using rtype::engine::DataFileDecimalOutOfRangeException;
using rtype::engine::DataFileFlagExpectedException;
using rtype::engine::DataFileRangeExpectedException;
using rtype::engine::DataFileSyntaxException;
using rtype::engine::DataFileValueOutOfRangeException;
using rtype::engine::DecimalRange;
using rtype::engine::MissingDataFileKeyException;
using rtype::engine::UnknownDataFileChoiceException;
using rtype::engine::UnknownDataFileKeyException;
using ::testing::HasSubstr;
using ::testing::ThrowsMessage;

/**
 * Given a file that lists a planet, then walls
 * When it is read
 * Then the background has the name of its file, and its layers in that order
 */
TEST(BackgroundReading, ReadsTheLayersInTheOrderOfTheFile) {
  const BackgroundDescription background =
      backgroundReading(A_PLANET_THEN_WALLS);

  ASSERT_EQ(background.layers.size(), TWO_LAYERS);
  EXPECT_EQ(background.name, READ_BACKGROUND_NAME);
  EXPECT_EQ(background.layers[PLANET_LAYER].name, PLANET_NAME);
  EXPECT_EQ(background.layers[WALL_LAYER].name, WALL_NAME);
}

/**
 * Given a planet whose motion gives its parallax, autoscroll, repeat and warp
 * When it is read
 * Then its motion holds each as written
 */
TEST(BackgroundReading, ReadsTheMotionOfALayer) {
  const BackgroundDescription background =
      backgroundReading(A_PLANET_THEN_WALLS);

  ASSERT_EQ(background.layers.size(), TWO_LAYERS);
  const LayerMotion motion = background.layers[PLANET_LAYER].motion;
  EXPECT_FLOAT_EQ(motion.parallax, HALF_THE_SCROLL);
  EXPECT_FLOAT_EQ(motion.autoscroll, PLANET_AUTOSCROLL);
  EXPECT_FLOAT_EQ(motion.repeat, PLANET_REPEAT);
  EXPECT_TRUE(motion.warps);
}

/**
 * Given walls that give no motion
 * When they are read
 * Then they follow no scroll, move by nothing, never repeat and do not warp
 */
TEST(BackgroundReading, ALayerWithoutMotionStaysWhereItIs) {
  const BackgroundDescription background =
      backgroundReading(A_PLANET_THEN_WALLS);

  ASSERT_EQ(background.layers.size(), TWO_LAYERS);
  const LayerMotion motion = background.layers[WALL_LAYER].motion;
  EXPECT_FLOAT_EQ(motion.parallax, 0.0F);
  EXPECT_FLOAT_EQ(motion.autoscroll, 0.0F);
  EXPECT_FLOAT_EQ(motion.repeat, 0.0F);
  EXPECT_FALSE(motion.warps);
}

/**
 * Given walls marked as the entrance, after a planet that is not
 * When they are read
 * Then the walls alone are the entrance
 */
TEST(BackgroundReading, ReadsWhichLayerIsTheEntrance) {
  const BackgroundDescription background =
      backgroundReading(A_PLANET_THEN_WALLS);

  ASSERT_EQ(background.layers.size(), TWO_LAYERS);
  EXPECT_FALSE(background.layers[PLANET_LAYER].entrance);
  EXPECT_TRUE(background.layers[WALL_LAYER].entrance);
}

/**
 * Given a planet whose image names its file and its place
 * When it is read
 * Then its content is that image, at that place
 */
TEST(BackgroundReading, ReadsAnImage) {
  const BackgroundDescription background =
      backgroundReading(A_PLANET_THEN_WALLS);

  ASSERT_EQ(background.layers.size(), TWO_LAYERS);
  const auto image = contentOf<ImageContent>(background.layers[PLANET_LAYER]);
  EXPECT_EQ(image.file, PLANET_FILE);
  EXPECT_EQ(image.topLeft, PLANET_TOP_LEFT);
}

/**
 * Given an image that gives no place
 * When it is read
 * Then its top-left corner lies at the origin of its layer
 */
TEST(BackgroundReading, AnImageWithoutAPlaceLiesAtTheOrigin) {
  const LayerDescription layer = layerReading(AN_IMAGE_WITHOUT_A_PLACE);

  EXPECT_EQ(contentOf<ImageContent>(layer).topLeft, TOP_LEFT_OF_A_FRAME);
}

/**
 * Given walls whose tiles name a project, one of its layers and a tileset
 * When they are read
 * Then their content is those tiles
 */
TEST(BackgroundReading, ReadsTiles) {
  const BackgroundDescription background =
      backgroundReading(A_PLANET_THEN_WALLS);

  ASSERT_EQ(background.layers.size(), TWO_LAYERS);
  const auto tiles = contentOf<MapContent>(background.layers[WALL_LAYER]);
  EXPECT_EQ(tiles.kind, MapLayerKind::Tiles);
  EXPECT_EQ(tiles.project, FOSSE_PROJECT);
  EXPECT_EQ(tiles.layer, NEAR_LAYER_OF_THE_PROJECT);
  EXPECT_EQ(tiles.tileset, NEAR_TILESET);
}

/**
 * Given crates whose props name a project, one of its layers and a tileset
 * When they are read
 * Then their content is those props
 */
TEST(BackgroundReading, ReadsProps) {
  const LayerDescription layer = layerReading(CRATES);

  const auto props = contentOf<MapContent>(layer);
  EXPECT_EQ(props.kind, MapLayerKind::Props);
  EXPECT_EQ(props.project, FOSSE_PROJECT);
  EXPECT_EQ(props.layer, PROP_LAYER_OF_THE_PROJECT);
  EXPECT_EQ(props.tileset, PROP_TILESET);
}

/**
 * Given the far stars, 441 pixels born anywhere on the background
 * When they are read
 * Then they are a population of 441, of no group, born on the whole
 * background
 */
TEST(BackgroundReading, ReadsAPopulation) {
  const ParticleContent stars = particlesReading(FAR_STARS);

  EXPECT_EQ(stars.population, FAR_STAR_COUNT);
  EXPECT_TRUE(stars.group.empty());
  EXPECT_EQ(stars.area, WHOLE_BACKGROUND);
}

/**
 * Given the far stars, deep blue pixels that twinkle in blue one step in
 * eight and leave a deep blue streak in a warp
 * When they are read
 * Then they hold those three colors of the palette and that count of steps
 */
TEST(BackgroundReading, ReadsThePixelOfAPopulationAndItsTwinkle) {
  const ParticleContent stars = particlesReading(FAR_STARS);

  EXPECT_EQ(stars.pixel, DEEP_BLUE_OF_THE_PALETTE);
  EXPECT_EQ(stars.twinkle, BLUE_OF_THE_PALETTE);
  EXPECT_EQ(stars.twinkleSteps, ONE_STEP_IN_EIGHT);
  EXPECT_EQ(stars.warpTrail, DEEP_BLUE_OF_THE_PALETTE);
  EXPECT_TRUE(stars.sprites.empty());
}

/**
 * Given a population that gives only its count, its area and its pixel
 * When it is read
 * Then it never twinkles and leaves no streak
 */
TEST(BackgroundReading, APopulationThatSaysNoMoreNeverTwinkles) {
  const ParticleContent stars = particlesReading(PLAIN_STARS);

  EXPECT_EQ(stars.pixel, PALE_BLUE_OF_THE_PALETTE);
  EXPECT_FALSE(stars.twinkle.has_value());
  EXPECT_EQ(stars.twinkleSteps, 0);
  EXPECT_FALSE(stars.warpTrail.has_value());
}

/**
 * Given the near stars, whose first sprite gives its file, 2 frames, a weight
 * of 4 and its anchor
 * When they are read
 * Then they are drawn with no pixel, and their first sprite holds each as
 * written, with no frame rate
 */
TEST(BackgroundReading, ReadsTheSpritesOfParticles) {
  const ParticleContent stars = particlesReading(NEAR_STARS);

  ASSERT_EQ(stars.sprites.size(), TWO_SPRITES);
  const ParticleSprite& sprite = stars.sprites.front();
  EXPECT_FALSE(stars.pixel.has_value());
  EXPECT_EQ(sprite.file, NEAR_STAR_FILE);
  EXPECT_EQ(sprite.frames, TWO_FRAMES);
  EXPECT_DOUBLE_EQ(sprite.weight, FOUR_TIMES_AS_LIKELY);
  EXPECT_EQ(sprite.anchor, MIDDLE_OF_A_NEAR_STAR);
  EXPECT_FALSE(sprite.framesPerSecond.has_value());
}

/**
 * Given the near stars, whose second sprite gives only its file
 * When they are read
 * Then that sprite holds one frame, a weight of 1 and its anchor on its
 * top-left pixel
 */
TEST(BackgroundReading, ASpriteThatSaysNoMoreIsOneFrame) {
  const ParticleContent stars = particlesReading(NEAR_STARS);

  ASSERT_EQ(stars.sprites.size(), TWO_SPRITES);
  const ParticleSprite& sprite = stars.sprites.back();
  EXPECT_EQ(sprite.frames, ONE_FRAME);
  EXPECT_DOUBLE_EQ(sprite.weight, AS_LIKELY_AS_ANY);
  EXPECT_EQ(sprite.anchor, TOP_LEFT_OF_A_FRAME);
}

/**
 * Given the asteroids, whose sprite spins at 6 to 18 frames per second
 * When they are read
 * Then their sprite holds that frame rate
 */
TEST(BackgroundReading, ReadsTheFrameRateOfASprite) {
  const ParticleContent asteroids = particlesReading(ASTEROIDS);

  ASSERT_EQ(asteroids.sprites.size(), 1U);
  const DecimalRange spin =
      asteroids.sprites.front().framesPerSecond.value_or(DecimalRange{});
  EXPECT_DOUBLE_EQ(spin.lowest, SLOWEST_SPIN);
  EXPECT_DOUBLE_EQ(spin.highest, FASTEST_SPIN);
}

/**
 * Given the asteroids, particles of the group of the sky events with a weight
 * of 1.5, born right of the background
 * When they are read
 * Then they are no population, and hold that group, weight and area
 */
TEST(BackgroundReading, ReadsTheGroupOfParticles) {
  const ParticleContent asteroids = particlesReading(ASTEROIDS);

  EXPECT_EQ(asteroids.population, 0U);
  EXPECT_EQ(asteroids.group, SKY_EVENT_GROUP);
  EXPECT_DOUBLE_EQ(asteroids.weight, HALF_AS_LIKELY_AGAIN);
  EXPECT_EQ(asteroids.area, RIGHT_OF_THE_BACKGROUND);
}

/**
 * Given the asteroids, which give their speed, angle, drift and margin
 * When they are read
 * Then they hold each as written
 */
TEST(BackgroundReading, ReadsHowTheParticlesOfAGroupFly) {
  const ParticleContent asteroids = particlesReading(ASTEROIDS);

  EXPECT_DOUBLE_EQ(asteroids.speed.lowest, SLOWEST_ASTEROID);
  EXPECT_DOUBLE_EQ(asteroids.speed.highest, FASTEST_ASTEROID);
  EXPECT_DOUBLE_EQ(asteroids.angle.lowest, HIGHEST_ASTEROID_ANGLE);
  EXPECT_DOUBLE_EQ(asteroids.angle.highest, LOWEST_ASTEROID_ANGLE);
  EXPECT_FLOAT_EQ(asteroids.drift, ASTEROID_DRIFT);
  EXPECT_FLOAT_EQ(asteroids.margin, ASTEROID_MARGIN);
}

/**
 * Given the asteroids, which live 2000 to 3000.5 milliseconds
 * When they are read
 * Then their life goes from 2 seconds to 3 seconds and half a millisecond
 */
TEST(BackgroundReading, ReadsTheLifeOfParticlesInMilliseconds) {
  const ParticleContent asteroids = particlesReading(ASTEROIDS);

  ASSERT_TRUE(asteroids.life.has_value());
  const DurationRange life = asteroids.life.value_or(DurationRange{});
  EXPECT_EQ(life.shortest, SHORTEST_ASTEROID_LIFE);
  EXPECT_EQ(life.longest, LONGEST_ASTEROID_LIFE);
}

/**
 * Given particles of a group that give only their group, area, look and speed
 * When they are read
 * Then they fly to the left with a weight of 1, no drift, no margin, no life
 * and no trail
 */
TEST(BackgroundReading, ParticlesOfAGroupThatSayNoMoreFlyToTheLeft) {
  const ParticleContent asteroids = particlesReading(PLAIN_ASTEROIDS);

  EXPECT_DOUBLE_EQ(asteroids.angle.lowest, TO_THE_LEFT);
  EXPECT_DOUBLE_EQ(asteroids.angle.highest, TO_THE_LEFT);
  EXPECT_DOUBLE_EQ(asteroids.weight, AS_LIKELY_AS_ANY);
  EXPECT_FLOAT_EQ(asteroids.drift, 0.0F);
  EXPECT_FLOAT_EQ(asteroids.margin, 0.0F);
  EXPECT_FALSE(asteroids.life.has_value());
  EXPECT_FALSE(asteroids.trail.has_value());
}

/**
 * Given the shooting stars, whose trail gives its longest, its growth, its
 * shrink and two colors
 * When they are read
 * Then their trail holds each as written, its colors in the order of the file
 */
TEST(BackgroundReading, ReadsATrail) {
  const ParticleContent stars = particlesReading(SHOOTING_STARS);

  ASSERT_TRUE(stars.trail.has_value());
  const ParticleTrail trail = stars.trail.value_or(ParticleTrail{});
  ASSERT_EQ(trail.stops.size(), TWO_STOPS);
  EXPECT_FLOAT_EQ(trail.longest, LONGEST_TRAIL);
  EXPECT_FLOAT_EQ(trail.growth, TRAIL_GROWTH);
  EXPECT_FLOAT_EQ(trail.shrink, TRAIL_SHRINK);
  EXPECT_FLOAT_EQ(trail.stops.front().until, HALF_THE_TRAIL);
  EXPECT_EQ(trail.stops.front().color, WHITE_OF_THE_PALETTE);
  EXPECT_FLOAT_EQ(trail.stops.back().until, THE_WHOLE_TRAIL);
  EXPECT_EQ(trail.stops.back().color, DEEP_BLUE_OF_THE_PALETTE);
}

/**
 * Given a group of at most 4 particles, born first after 600 ms, then every
 * 800 to 2600 ms
 * When it is read
 * Then the background holds that group, with each as written
 */
TEST(BackgroundReading, ReadsAnEmissionGroup) {
  const BackgroundDescription background = backgroundReading(
      std::string{PARTICLE_LAYER_START} + std::string{PLAIN_ASTEROIDS});

  ASSERT_EQ(background.groups.size(), ONE_GROUP);
  const EmissionGroupDescription& group = background.groups.front();
  EXPECT_EQ(group.name, SKY_EVENT_GROUP);
  EXPECT_EQ(group.mostAlive, FOUR_AT_ONCE);
  EXPECT_EQ(group.first, FIRST_BIRTH);
  EXPECT_EQ(group.every.shortest, SHORTEST_BIRTH_INTERVAL);
  EXPECT_EQ(group.every.longest, LONGEST_BIRTH_INTERVAL);
}

/**
 * Given a file that lists no group
 * When it is read
 * Then the background holds none
 */
TEST(BackgroundReading, AFileWithoutGroupsHoldsNone) {
  const BackgroundDescription background =
      backgroundReading(A_PLANET_THEN_WALLS);

  EXPECT_TRUE(background.groups.empty());
}

/**
 * Given a file whose second line has a key and no value
 * When it is read
 * Then a DataFileSyntaxException is thrown
 */
TEST(BackgroundReading, RefusesATextThatIsNotToml) {
  EXPECT_THROW(static_cast<void>(backgroundReading(NOT_TOML)),
               DataFileSyntaxException);
}

/**
 * Given a file that lists no layer
 * When it is read
 * Then a MissingDataFileKeyException says layer is missing
 */
TEST(BackgroundReading, RefusesAFileWithoutLayers) {
  EXPECT_THAT([] { static_cast<void>(backgroundReading(NO_LAYER)); },
              ThrowsMessage<MissingDataFileKeyException>(
                  HasSubstr(std::string{MISSING_LAYERS_TEXT})));
}

/**
 * Given a file with a key one of its tables does not know
 * When it is read
 * Then reading stops on that key
 */
TEST_P(BackgroundWithAnUnknownKey, IsRefused) {
  EXPECT_THROW(static_cast<void>(backgroundReading(GetParam())),
               UnknownDataFileKeyException);
}

INSTANTIATE_TEST_SUITE_P(
    InEachTable, BackgroundWithAnUnknownKey,
    ::testing::Values(UNKNOWN_KEY_OF_THE_ROOT, UNKNOWN_KEY_OF_A_LAYER,
                      UNKNOWN_KEY_OF_A_MOTION, UNKNOWN_KEY_OF_AN_IMAGE,
                      UNKNOWN_KEY_OF_TILES, UNKNOWN_KEY_OF_A_GROUP));

/**
 * Given particles with a key they do not know: a misspelt one in one of their
 * tables, a key of a group in a population, or a key of a population in a
 * group
 * When they are read
 * Then reading stops on that key
 */
TEST_P(ParticlesWithAnUnknownKey, AreRefused) {
  EXPECT_THROW(static_cast<void>(particlesReading(GetParam())),
               UnknownDataFileKeyException);
}

INSTANTIATE_TEST_SUITE_P(
    InEachTable, ParticlesWithAnUnknownKey,
    ::testing::Values(UNKNOWN_KEY_OF_A_POPULATION, UNKNOWN_KEY_OF_AN_AREA,
                      UNKNOWN_KEY_OF_A_SPRITE, A_SPEED_IN_A_POPULATION,
                      A_WARP_TRAIL_IN_A_GROUP, UNKNOWN_KEY_OF_A_TRAIL,
                      UNKNOWN_KEY_OF_A_TRAIL_STOP));

/**
 * Given a layer that holds a motion and no content
 * When it is read
 * Then an InconsistentBackgroundException says a layer holds exactly one
 * content
 */
TEST(BackgroundReading, RefusesALayerWithoutContent) {
  EXPECT_THAT([] { static_cast<void>(layerReading(NO_CONTENT)); },
              ThrowsMessage<InconsistentBackgroundException>(
                  HasSubstr(std::string{ONE_CONTENT_REFUSAL})));
}

/**
 * Given a layer that holds an image and tiles
 * When it is read
 * Then an InconsistentBackgroundException says a layer holds exactly one
 * content
 */
TEST(BackgroundReading, RefusesALayerWithTwoContents) {
  EXPECT_THAT([] { static_cast<void>(layerReading(AN_IMAGE_AND_TILES)); },
              ThrowsMessage<InconsistentBackgroundException>(
                  HasSubstr(std::string{ONE_CONTENT_REFUSAL})));
}

/**
 * Given two layers that are both the entrance
 * When they are read
 * Then an InconsistentBackgroundException names the entrance of the second
 */
TEST(BackgroundReading, RefusesASecondEntrance) {
  EXPECT_THAT([] { static_cast<void>(backgroundReading(TWO_ENTRANCES)); },
              ThrowsMessage<InconsistentBackgroundException>(
                  HasSubstr(std::string{SECOND_ENTRANCE_REFUSAL})));
}

/**
 * Given an image whose file climbs out of the content folders
 * When it is read
 * Then an InconsistentBackgroundException says a file is named by its asset
 * id
 */
TEST(BackgroundReading, RefusesAFileThatIsNotAnAssetId) {
  EXPECT_THAT(
      [] { static_cast<void>(layerReading(A_FILE_OUTSIDE_THE_ASSETS)); },
      ThrowsMessage<InconsistentBackgroundException>(
          HasSubstr(std::string{FILE_REFUSAL})));
}

/**
 * Given a motion whose warps is written 1
 * When it is read
 * Then a DataFileFlagExpectedException is thrown
 */
TEST(BackgroundReading, RefusesAFlagWrittenAsANumber) {
  EXPECT_THROW(static_cast<void>(layerReading(WARPS_WRITTEN_AS_A_NUMBER)),
               DataFileFlagExpectedException);
}

/**
 * Given a motion whose parallax is 5, over the 4 a layer may follow
 * When it is read
 * Then a DataFileDecimalOutOfRangeException is thrown
 */
TEST(BackgroundReading, RefusesAParallaxOverItsRange) {
  EXPECT_THROW(static_cast<void>(layerReading(A_PARALLAX_OF_FIVE)),
               DataFileDecimalOutOfRangeException);
}

/**
 * Given particles that hold both a population and a group
 * When they are read
 * Then an InconsistentBackgroundException says they hold either
 */
TEST(BackgroundReading, RefusesAPopulationThatNamesAGroup) {
  EXPECT_THAT(
      [] { static_cast<void>(particlesReading(A_POPULATION_OF_A_GROUP)); },
      ThrowsMessage<InconsistentBackgroundException>(
          HasSubstr(std::string{EMISSION_REFUSAL})));
}

/**
 * Given particles that hold neither a population nor a group
 * When they are read
 * Then an InconsistentBackgroundException says they hold either
 */
TEST(BackgroundReading, RefusesParticlesThatAreNeitherPopulationNorGroup) {
  EXPECT_THAT(
      [] { static_cast<void>(particlesReading(NEITHER_POPULATION_NOR_GROUP)); },
      ThrowsMessage<InconsistentBackgroundException>(
          HasSubstr(std::string{EMISSION_REFUSAL})));
}

/**
 * Given particles of a group the file declares nowhere
 * When they are read
 * Then an InconsistentBackgroundException names their key group
 */
TEST(BackgroundReading, RefusesAGroupTheFileDoesNotDeclare) {
  EXPECT_THAT(
      [] { static_cast<void>(particlesReading(A_GROUP_NOTHING_DECLARES)); },
      ThrowsMessage<InconsistentBackgroundException>(
          HasSubstr(std::string{UNKNOWN_GROUP_REFUSAL})));
}

/**
 * Given a file that declares two groups of the same name
 * When it is read
 * Then an InconsistentBackgroundException names the name of the second
 */
TEST(BackgroundReading, RefusesTwoGroupsOfOneName) {
  EXPECT_THAT(
      [] { static_cast<void>(backgroundReading(TWO_GROUPS_OF_ONE_NAME)); },
      ThrowsMessage<InconsistentBackgroundException>(
          HasSubstr(std::string{SHARED_GROUP_NAME_REFUSAL})));
}

/**
 * Given particles drawn with both a pixel and a sprite
 * When they are read
 * Then an InconsistentBackgroundException says they hold either
 */
TEST(BackgroundReading, RefusesAPixelBesideSprites) {
  EXPECT_THAT([] { static_cast<void>(particlesReading(A_PIXEL_AND_A_SPRITE)); },
              ThrowsMessage<InconsistentBackgroundException>(
                  HasSubstr(std::string{LOOK_REFUSAL})));
}

/**
 * Given particles drawn with neither a pixel nor a sprite
 * When they are read
 * Then an InconsistentBackgroundException says they hold either
 */
TEST(BackgroundReading, RefusesParticlesWithoutALook) {
  EXPECT_THAT(
      [] { static_cast<void>(particlesReading(NEITHER_PIXEL_NOR_SPRITE)); },
      ThrowsMessage<InconsistentBackgroundException>(
          HasSubstr(std::string{LOOK_REFUSAL})));
}

/**
 * Given a pixel with a twinkle color and no count of twinkle steps
 * When it is read
 * Then an InconsistentBackgroundException says what the twinkle color goes
 * with
 */
TEST(BackgroundReading, RefusesATwinkleColorThatNeverShows) {
  EXPECT_THAT(
      [] {
        static_cast<void>(particlesReading(A_TWINKLE_COLOR_WITHOUT_STEPS));
      },
      ThrowsMessage<InconsistentBackgroundException>(
          HasSubstr(std::string{TWINKLE_REFUSAL})));
}

/**
 * Given a pixel that twinkles one step in eight, with no twinkle color
 * When it is read
 * Then an InconsistentBackgroundException says what the twinkle color goes
 * with
 */
TEST(BackgroundReading, RefusesAPixelThatTwinklesInNoColor) {
  EXPECT_THAT(
      [] {
        static_cast<void>(
            particlesReading(A_PIXEL_THAT_TWINKLES_WITHOUT_A_COLOR));
      },
      ThrowsMessage<InconsistentBackgroundException>(
          HasSubstr(std::string{TWINKLE_REFUSAL})));
}

/**
 * Given particles drawn with a sprite, with a twinkle color
 * When they are read
 * Then an InconsistentBackgroundException says what the twinkle color goes
 * with
 */
TEST(BackgroundReading, RefusesATwinkleColorForSprites) {
  EXPECT_THAT(
      [] { static_cast<void>(particlesReading(A_TWINKLE_COLOR_FOR_SPRITES)); },
      ThrowsMessage<InconsistentBackgroundException>(
          HasSubstr(std::string{TWINKLE_REFUSAL})));
}

/**
 * Given particles of a group with no life and no speed
 * When they are read
 * Then an InconsistentBackgroundException says how the particles of a group
 * end
 */
TEST(BackgroundReading, RefusesParticlesOfAGroupThatNeverEnd) {
  EXPECT_THAT(
      [] { static_cast<void>(particlesReading(PARTICLES_THAT_STAY_FOR_GOOD)); },
      ThrowsMessage<InconsistentBackgroundException>(
          HasSubstr(std::string{ENDLESS_REFUSAL})));
}

/**
 * Given particles of a group with no life, whose speed may be 0
 * When they are read
 * Then an InconsistentBackgroundException says how the particles of a group
 * end
 */
TEST(BackgroundReading, RefusesParticlesOfAGroupThatMayNeverEnd) {
  EXPECT_THAT(
      [] {
        static_cast<void>(particlesReading(PARTICLES_THAT_MAY_STAY_FOR_GOOD));
      },
      ThrowsMessage<InconsistentBackgroundException>(
          HasSubstr(std::string{ENDLESS_REFUSAL})));
}

/**
 * Given particles of a group with no speed, which live one second
 * When they are read
 * Then they are accepted, with that life
 */
TEST(BackgroundReading, ParticlesThatStayEndWithTheirLife) {
  const ParticleContent flares =
      particlesReading(PARTICLES_THAT_STAY_FOR_A_SECOND);

  ASSERT_TRUE(flares.life.has_value());
  EXPECT_EQ(flares.life.value_or(DurationRange{}).longest,
            std::chrono::seconds{1});
}

/**
 * Given a pixel whose color is no name of the palette
 * When it is read
 * Then an UnknownDataFileChoiceException names the key pixel
 */
TEST(BackgroundReading, RefusesAColorOutsideThePalette) {
  EXPECT_THAT(
      [] { static_cast<void>(particlesReading(A_COLOR_OUTSIDE_THE_PALETTE)); },
      ThrowsMessage<UnknownDataFileChoiceException>(
          HasSubstr(std::string{PIXEL_KEY_NAME})));
}

/**
 * Given a population of 0 particles, one under the 1 it holds at least
 * When it is read
 * Then a DataFileValueOutOfRangeException is thrown
 */
TEST(BackgroundReading, RefusesAPopulationOfNone) {
  EXPECT_THROW(static_cast<void>(particlesReading(A_POPULATION_OF_NONE)),
               DataFileValueOutOfRangeException);
}

/**
 * Given a speed written with its fastest first
 * When it is read
 * Then a DataFileRangeExpectedException is thrown
 */
TEST(BackgroundReading, RefusesARangeWithItsHighestFirst) {
  EXPECT_THROW(
      static_cast<void>(particlesReading(A_SPEED_WITH_ITS_FASTEST_FIRST)),
      DataFileRangeExpectedException);
}

/**
 * Given a file of 32 layers, the most a background holds
 * When it is read
 * Then the background holds the 32
 */
TEST(BackgroundReading, ReadsAsManyLayersAsABackgroundHolds) {
  const BackgroundDescription background =
      backgroundReading(layersCounting(MOST_LAYERS));

  EXPECT_EQ(background.layers.size(), MOST_LAYERS);
}

/**
 * Given a file of 33 layers, one more than a background holds
 * When it is read
 * Then a TooManyBackgroundTablesException says layer holds more than 32
 */
TEST(BackgroundReading, RefusesOneLayerTooMany) {
  EXPECT_THAT(
      [] {
        static_cast<void>(
            backgroundReading(layersCounting(ONE_LAYER_TOO_MANY)));
      },
      ThrowsMessage<TooManyBackgroundTablesException>(
          HasSubstr(std::string{TOO_MANY_LAYERS_REFUSAL})));
}

/**
 * Given a background that names the music of its level
 * When it is read
 * Then it holds that music
 */
TEST(BackgroundReading, ReadsTheMusicItNames) {
  const BackgroundDescription background =
      backgroundReading(A_BACKGROUND_WITH_ITS_MUSIC);

  EXPECT_EQ(background.music, THE_MUSIC_IT_NAMES);
}

/**
 * Given a background that names no music
 * When it is read
 * Then it holds none, so a game plays its own music
 */
TEST(BackgroundReading, ABackgroundWithoutMusicHoldsNone) {
  const BackgroundDescription background =
      backgroundReading(A_PLANET_THEN_WALLS);

  EXPECT_TRUE(background.music.empty());
}

/**
 * Given a background whose music lies outside the assets
 * When it is read
 * Then an InconsistentBackgroundException says a file is named by its asset
 * id
 */
TEST(BackgroundReading, RefusesAMusicThatIsNotAnAssetId) {
  EXPECT_THAT(
      [] { static_cast<void>(backgroundReading(A_MUSIC_OUTSIDE_THE_ASSETS)); },
      ThrowsMessage<InconsistentBackgroundException>(
          HasSubstr(std::string{MUSIC_REFUSAL})));
}

#include <gtest/gtest.h>
#include <string>
#include "BackgroundDescription.hpp"
#include "BackgroundReadingFixtures.hpp"
#include "BackgroundReadingTestConstants.hpp"
#include "BuiltLayers.hpp"
#include "EmissionGroupTestConstants.hpp"
#include "LayerBuilding.hpp"
#include "LayerTestConstants.hpp"
#include "LayeredBackgroundFixture.hpp"
#include "LayeredBackgroundTestConstants.hpp"
#include "ParticleLayerTestConstants.hpp"
#include "RecordingLayerCanvas.hpp"

using rtype::client::BackgroundDescription;
using rtype::client::BuiltLayers;
using rtype::client::layersOf;

/**
 * Given a sky of two layers, particles then an image
 * When its layers are built
 * Then there is one content per layer, in the order of the file: the first
 * plots pixels, the second draws frames
 */
TEST(LayerBuilding, BuildsWhatEachLayerShowsInTheOrderOfTheFile) {
  const BackgroundDescription sky = backgroundReading(A_SMALL_SKY);
  RecordingLayerCanvas stars;
  RecordingLayerCanvas planet;

  const BuiltLayers built = layersOf(sky, shippedSources());

  ASSERT_EQ(built.contents.size(), LAYERS_OF_THE_SMALL_SKY);
  built.contents[STAR_LAYER]->paint(stars, AT_ITS_ORIGIN);
  built.contents[PLANET_LAYER_OF_THE_SKY]->paint(planet, AT_ITS_ORIGIN);
  EXPECT_FALSE(stars.plots().empty());
  EXPECT_TRUE(stars.frames().empty());
  EXPECT_FALSE(planet.frames().empty());
}

/**
 * Given a background whose particles name the group of the sky events
 * When its layers are built, and the first deadline of the group passes
 * Then the group gives birth in that layer of particles
 */
TEST(LayerBuilding, GivesAGroupTheLayersOfParticlesThatNameIt) {
  const BackgroundDescription events = backgroundReading(
      std::string{PARTICLE_LAYER_START} + std::string{LEFTWARD_PARTICLES});
  RecordingLayerCanvas canvas;

  BuiltLayers built = layersOf(events, shippedSources());
  ASSERT_EQ(built.groups.size(), 1U);
  ASSERT_EQ(built.contents.size(), 1U);
  built.groups.front().advance(JUST_SHORT_OF_THE_FIRST_EVENT);
  built.groups.front().advance(PAST_THE_FIRST_EVENT);
  built.contents.front()->paint(canvas, AT_ITS_ORIGIN);

  EXPECT_EQ(built.groups.front().aliveCount(), 1U);
  EXPECT_EQ(canvas.plots().size(), 1U);
}

/**
 * Given a layer of tiles whose LDtk project no folder holds
 * When the layers are built
 * Then nothing is thrown, and that layer shows nothing
 */
TEST(LayerBuilding, ALayerWhoseProjectCannotBeReadShowsNothing) {
  const BackgroundDescription walls =
      backgroundReading(WALLS_OF_A_MISSING_PROJECT);
  RecordingLayerCanvas canvas;

  const BuiltLayers built = layersOf(walls, shippedSources());

  ASSERT_EQ(built.contents.size(), 1U);
  built.contents.front()->paint(canvas, AT_ITS_ORIGIN);
  EXPECT_TRUE(canvas.tiles().empty());
}

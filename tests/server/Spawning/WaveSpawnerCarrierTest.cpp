#include <gtest/gtest.h>
#include <vector>
#include "EntitySpawned.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "Position.hpp"
#include "WaveEntryPosition.hpp"
#include "WavePlanFixtures.hpp"
#include "WaveSpawner.hpp"
#include "WaveSpawnerCarrierFixtures.hpp"
#include "WaveSpawnerCarrierTestConstants.hpp"
#include "WaveSpawnerScene.hpp"
#include "WaveSpawnerTestConstants.hpp"
#include "game/Waves/WaveTestConstants.hpp"

using rtype::game::Position;
using rtype::game::waveEntryPosition;
using rtype::game::testing::TICKS_TO_THE_WHOLE_FIRST_WAVE;
using rtype::network::EntitySpawned;
using rtype::server::WaveSpawner;
using rtype::server::testing::CARRIERS_OF_FOUR_WAVES;
using rtype::server::testing::carriersIn;
using rtype::server::testing::FIRST_PLAYER;
using rtype::server::testing::playClearingTheSky;
using rtype::server::testing::QUARTER_OF_THE_HEIGHT;
using rtype::server::testing::SPAWNER_SEED;
using rtype::server::testing::testWavePlan;
using rtype::server::testing::TICKS_OF_FOUR_WAVES;
using rtype::server::testing::wavePlanWithACarrier;
using rtype::server::testing::WaveSpawnerScene;

/**
 * Given a spawner whose plan brings the crystal carrier every other wave
 * When the whole first wave has appeared
 * Then no carrier was announced: the first wave brings none
 */
TEST(WaveSpawner, NoCarrierDuringTheFirstWave) {
  WaveSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  WaveSpawner spawner =
      scene.makeSpawnerWith(SPAWNER_SEED, wavePlanWithACarrier());

  WaveSpawnerScene::runTicks(spawner, TICKS_TO_THE_WHOLE_FIRST_WAVE);

  EXPECT_TRUE(carriersIn(scene.spawnsSent()).empty());
}

/**
 * Given a spawner whose plan brings the crystal carrier every other wave
 * When the waves 2 to 5 begin, the sky cleared at each tick
 * Then two carriers came, with the second and the fourth waves
 */
TEST(WaveSpawner, ACarrierComesWithEveryOtherWave) {
  WaveSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  WaveSpawner spawner =
      scene.makeSpawnerWith(SPAWNER_SEED, wavePlanWithACarrier());

  playClearingTheSky(scene, spawner, TICKS_OF_FOUR_WAVES);

  EXPECT_EQ(carriersIn(scene.spawnsSent()).size(), CARRIERS_OF_FOUR_WAVES);
}

/**
 * Given a spawner whose plan brings the crystal carrier, its first wave over
 * When the second wave begins
 * Then the carrier enters on the right edge, a quarter of the way down
 */
TEST(WaveSpawner, ACarrierEntersAQuarterOfTheWayDown) {
  WaveSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  WaveSpawner spawner =
      scene.makeSpawnerWith(SPAWNER_SEED, wavePlanWithACarrier());
  WaveSpawnerScene::runTicks(spawner, TICKS_TO_THE_WHOLE_FIRST_WAVE);
  scene.clearTheSky();

  WaveSpawnerScene::runTicks(spawner, 1);

  const std::vector<EntitySpawned> carriers = carriersIn(scene.spawnsSent());
  ASSERT_EQ(carriers.size(), 1U);
  const Position quarter = waveEntryPosition(QUARTER_OF_THE_HEIGHT);
  EXPECT_EQ(carriers.front().x, quarter.x);
  EXPECT_EQ(carriers.front().y, quarter.y);
}

/**
 * Given a spawner whose plan names no carrier
 * When the waves 2 to 5 begin, the sky cleared at each tick
 * Then no carrier came
 */
TEST(WaveSpawner, APlanWithoutACarrierBringsNone) {
  WaveSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  WaveSpawner spawner = scene.makeSpawnerWith(SPAWNER_SEED, testWavePlan());

  playClearingTheSky(scene, spawner, TICKS_OF_FOUR_WAVES);

  EXPECT_TRUE(carriersIn(scene.spawnsSent()).empty());
}

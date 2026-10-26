#include <gtest/gtest.h>
#include <algorithm>
#include <string>
#include <vector>
#include "BydoSpawner.hpp"
#include "BydoSpawnerScene.hpp"
#include "BydoSpawnerTestConstants.hpp"
#include "EntitySpawned.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "JournalFile.hpp"
#include "Logger.hpp"
#include "MovementPattern.hpp"
#include "PlayfieldConstants.hpp"
#include "ServerException.hpp"
#include "SpawningConstants.hpp"
#include "WavePlanFixtures.hpp"
#include "WorldState.hpp"
#include "game/Waves/WaveTestConstants.hpp"

using rtype::game::BYDO_ROW_MARGIN;
using rtype::game::PatternShape;
using rtype::game::PLAY_AREA_HEIGHT;
using rtype::game::PLAYFIELD_WIDTH;
using rtype::game::testing::TEST_WAVE_CURVE;
using rtype::game::testing::TICKS_OF_A_PAUSE;
using rtype::game::testing::TICKS_TO_THE_WHOLE_FIRST_WAVE;
using rtype::logging::Logger;
using rtype::logging::LogLevel;
using rtype::network::EntitySpawned;
using rtype::network::WireEntityType;
using rtype::server::BydoSpawner;
using rtype::server::EmptyEnemyCycleException;
using rtype::server::testing::BydoSpawnerScene;
using rtype::server::testing::FIRST_PLAYER;
using rtype::server::testing::FIRST_WAVE_LINE_TEXT;
using rtype::server::testing::OTHER_SPAWNER_SEED;
using rtype::server::testing::SECOND_PLAYER;
using rtype::server::testing::SPAWNER_SEED;
using rtype::server::testing::TICKS_TO_THE_FIRST_BYDO;
using rtype::server::testing::TICKS_TO_THE_SECOND_BYDO;
using rtype::server::testing::TWENTY_SECONDS_OF_TICKS;
using rtype::server::testing::wavePlanOf;
using rtype::server::testing::WEAVER_FIRST_CYCLE;

/**
 * Given a spawner and a player
 * When one tick less than the first pause has passed
 * Then no Bydo has appeared and nothing was sent
 */
TEST(BydoSpawner, NothingAppearsBeforeTheFirstPauseEnds) {
  BydoSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  BydoSpawnerScene::runTicks(spawner, TICKS_TO_THE_FIRST_BYDO - 1);

  EXPECT_EQ(scene.world().entityCount(), 0U);
  EXPECT_TRUE(scene.spawnsSent().empty());
}

/**
 * Given a spawner and a player
 * When the first pause has just passed
 * Then one Bydo is in the world and the player is sent one EntitySpawned for
 * it, past the right edge
 */
TEST(BydoSpawner, ABydoAppearsOffScreenWhenTheFirstPauseEnds) {
  BydoSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  BydoSpawnerScene::runTicks(spawner, TICKS_TO_THE_FIRST_BYDO);

  EXPECT_EQ(scene.world().entityCount(), 1U);
  const std::vector<EntitySpawned> spawns = scene.spawnsSent();
  ASSERT_EQ(spawns.size(), 1U);
  EXPECT_EQ(spawns.front().entityType, WireEntityType::Bydo);
  EXPECT_GT(spawns.front().x, PLAYFIELD_WIDTH);
  EXPECT_GE(spawns.front().y, BYDO_ROW_MARGIN);
  EXPECT_LE(spawns.front().y, PLAY_AREA_HEIGHT - BYDO_ROW_MARGIN);
}

/**
 * Given a spawner and two players
 * When the second Bydo of the first wave has appeared
 * Then two Bydos are in the world and each player was sent both, at the same
 * heights
 */
TEST(BydoSpawner, EveryPlayerIsToldAboutEveryBydo) {
  BydoSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  scene.connect(SECOND_PLAYER);
  BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  BydoSpawnerScene::runTicks(spawner, TICKS_TO_THE_SECOND_BYDO);

  EXPECT_EQ(scene.world().entityCount(), 2U);
  const std::vector<float> first = scene.heightsSentTo(FIRST_PLAYER);
  EXPECT_EQ(first.size(), 2U);
  EXPECT_EQ(first, scene.heightsSentTo(SECOND_PLAYER));
}

/**
 * Given two spawners started from the same seed
 * When their first wave appears
 * Then they announce the same heights in the same order, and a spawner started
 * from another seed announces other heights
 */
TEST(BydoSpawner, TheSeedDecidesTheHeights) {
  const std::vector<float> reference = BydoSpawnerScene::heightsWithin(
      SPAWNER_SEED, TICKS_TO_THE_WHOLE_FIRST_WAVE);
  ASSERT_EQ(reference.size(), TEST_WAVE_CURVE.firstWave.bydoCount);

  EXPECT_EQ(BydoSpawnerScene::heightsWithin(SPAWNER_SEED,
                                            TICKS_TO_THE_WHOLE_FIRST_WAVE),
            reference);
  EXPECT_NE(BydoSpawnerScene::heightsWithin(OTHER_SPAWNER_SEED,
                                            TICKS_TO_THE_WHOLE_FIRST_WAVE),
            reference);
}

/**
 * Given the Bydos of a first wave, announced from one seed
 * When their heights are compared
 * Then they are not all the same, so the height is not a fixed row
 */
TEST(BydoSpawner, HeightsVaryFromOneBydoToTheNext) {
  const std::vector<float> heights = BydoSpawnerScene::heightsWithin(
      SPAWNER_SEED, TICKS_TO_THE_WHOLE_FIRST_WAVE);

  EXPECT_GT(std::ranges::count_if(
                heights,
                [&heights](float height) { return height != heights.front(); }),
            0);
}

/**
 * Given no player connected
 * When the first pause passes
 * Then the Bydo is in the world and nothing is sent
 */
TEST(BydoSpawner, ABydoAppearsEvenWithNobodyToTell) {
  BydoSpawnerScene scene;
  BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  BydoSpawnerScene::runTicks(spawner, TICKS_TO_THE_FIRST_BYDO);

  EXPECT_EQ(scene.world().entityCount(), 1U);
  EXPECT_TRUE(scene.spawnsSent().empty());
}

/**
 * Given a log file as the only sink
 * When a spawner is started from a seed
 * Then the log holds a line with that seed, so the case can be replayed
 */
TEST(BydoSpawner, TheSeedIsLogged) {
  Logger::setLevel(LogLevel::Info);
  const JournalFile journal;
  Logger::setOutput({.toStderr = false, .filePath = journal.path()});
  BydoSpawnerScene scene;

  const BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  const std::vector<std::string> lines = journal.lines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find(std::to_string(SPAWNER_SEED)),
            std::string::npos);
}

/**
 * Given a log file as the only sink, and a spawner
 * When its first tick runs
 * Then the log holds a line naming the first wave, its 3 Bydos and the 2
 * seconds between two of them
 */
TEST(BydoSpawner, TheStartOfAWaveIsLogged) {
  Logger::setLevel(LogLevel::Info);
  const JournalFile journal;
  Logger::setOutput({.toStderr = false, .filePath = journal.path()});
  BydoSpawnerScene scene;
  BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  BydoSpawnerScene::runTicks(spawner, 1);

  const std::vector<std::string> lines = journal.lines();
  ASSERT_EQ(lines.size(), 2U);
  EXPECT_NE(lines.back().find(FIRST_WAVE_LINE_TEXT), std::string::npos);
}

/**
 * Given a spawner whose plan takes the Drifter then the Weaver in turn
 * When the first wave, three Bydos, has appeared
 * Then two Bydos go straight and one weaves: the first, the third and the
 * second
 */
TEST(BydoSpawner, TheBydosTakeTheKindsOfTheirPlanInTurn) {
  BydoSpawnerScene scene;
  BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  BydoSpawnerScene::runTicks(spawner, TICKS_TO_THE_WHOLE_FIRST_WAVE);

  const std::vector<PatternShape> shapes = scene.patternsOfTheBydos();
  EXPECT_EQ(std::ranges::count(shapes, PatternShape::Straight), 2);
  EXPECT_EQ(std::ranges::count(shapes, PatternShape::Wave), 1);
}

/**
 * Given a spawner whose plan names the Weaver first
 * When the first Bydo appears
 * Then it weaves: the cycle starts with the first kind the plan names
 */
TEST(BydoSpawner, TheCycleStartsWithTheFirstKindItNames) {
  BydoSpawnerScene scene;
  BydoSpawner spawner =
      scene.makeSpawnerWith(SPAWNER_SEED, wavePlanOf(WEAVER_FIRST_CYCLE));

  BydoSpawnerScene::runTicks(spawner, TICKS_TO_THE_FIRST_BYDO);

  EXPECT_EQ(scene.patternsOfTheBydos(), std::vector{PatternShape::Wave});
}

/**
 * Given a spawner whose plan takes the Drifter then the Weaver in turn, its
 * first wave of three Bydos gone from the world
 * When a pause passes and the first Bydo of the second wave appears
 * Then it weaves: it is the fourth of the cycle, which carries on from one
 * wave to the next
 */
TEST(BydoSpawner, TheCycleCarriesOnIntoTheNextWave) {
  BydoSpawnerScene scene;
  BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);
  BydoSpawnerScene::runTicks(spawner, TICKS_TO_THE_WHOLE_FIRST_WAVE);
  scene.clearTheSky();

  BydoSpawnerScene::runTicks(spawner, TICKS_OF_A_PAUSE);

  EXPECT_EQ(scene.patternsOfTheBydos(), std::vector{PatternShape::Wave});
}

/**
 * Given a spawner whose first wave appeared, its Bydos still in the world
 * When twenty more seconds pass
 * Then no other Bydo appears: the next wave waits for the world to hold no
 * Bydo
 */
TEST(BydoSpawner, TheNextWaveWaitsForTheBydosLeftInTheWorld) {
  BydoSpawnerScene scene;
  BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);
  BydoSpawnerScene::runTicks(spawner, TICKS_TO_THE_WHOLE_FIRST_WAVE);
  ASSERT_EQ(scene.world().entityCount(), TEST_WAVE_CURVE.firstWave.bydoCount);

  BydoSpawnerScene::runTicks(spawner, TWENTY_SECONDS_OF_TICKS);

  EXPECT_EQ(scene.world().entityCount(), TEST_WAVE_CURVE.firstWave.bydoCount);
}

/**
 * Given a wave plan that names no enemy
 * When a spawner is built on it
 * Then an EmptyEnemyCycleException is thrown
 */
TEST(BydoSpawner, RefusesAPlanWithoutEnemies) {
  BydoSpawnerScene scene;

  EXPECT_THROW(
      static_cast<void>(scene.makeSpawnerWith(SPAWNER_SEED, wavePlanOf({}))),
      EmptyEnemyCycleException);
}

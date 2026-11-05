#include <gtest/gtest.h>
#include <algorithm>
#include <string>
#include <string_view>
#include <vector>
#include "BehaviourConstants.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawned.hpp"
#include "EntityTypesTestConstants.hpp"
#include "GameException.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "JournalFile.hpp"
#include "Logger.hpp"
#include "PlayfieldConstants.hpp"
#include "ServerException.hpp"
#include "ShippedEntitySheets.hpp"
#include "SpawningConstants.hpp"
#include "WavePlanFixtures.hpp"
#include "WaveSpawner.hpp"
#include "WaveSpawnerScene.hpp"
#include "WaveSpawnerTestConstants.hpp"
#include "game/Waves/WaveTestConstants.hpp"

using rtype::game::ENEMY_ROW_MARGIN;
using rtype::game::PLAY_AREA_HEIGHT;
using rtype::game::PLAYFIELD_WIDTH;
using rtype::game::STRAIGHT_BEHAVIOUR_NAME;
using rtype::game::UnknownEntityTypeException;
using rtype::game::WEAVE_BEHAVIOUR_NAME;
using rtype::game::testing::DRIFTER_SHEET;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::TEST_WAVE_CURVE;
using rtype::game::testing::TICKS_OF_A_PAUSE;
using rtype::game::testing::TICKS_TO_THE_WHOLE_FIRST_WAVE;
using rtype::logging::Logger;
using rtype::logging::LogLevel;
using rtype::network::EntitySpawned;
using rtype::server::EmptyEnemyCycleException;
using rtype::server::WaveSpawner;
using rtype::server::testing::FIRST_PLAYER;
using rtype::server::testing::FIRST_WAVE_LINE_TEXT;
using rtype::server::testing::NO_SHEET_CYCLE;
using rtype::server::testing::oneWaveLevelWavePlan;
using rtype::server::testing::OTHER_SPAWNER_SEED;
using rtype::server::testing::SECOND_PLAYER;
using rtype::server::testing::SPAWNER_SEED;
using rtype::server::testing::TICKS_TO_THE_FIRST_BYDO;
using rtype::server::testing::TICKS_TO_THE_SECOND_BYDO;
using rtype::server::testing::TWENTY_SECONDS_OF_TICKS;
using rtype::server::testing::wavePlanOf;
using rtype::server::testing::WaveSpawnerScene;
using rtype::server::testing::WEAVER_FIRST_CYCLE;

/**
 * Given a spawner and a player
 * When one tick less than the first pause has passed
 * Then no Bydo has appeared and nothing was sent
 */
TEST(WaveSpawner, NothingAppearsBeforeTheFirstPauseEnds) {
  WaveSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  WaveSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  WaveSpawnerScene::runTicks(spawner, TICKS_TO_THE_FIRST_BYDO - 1);

  EXPECT_EQ(scene.world().entityCount(), 0U);
  EXPECT_TRUE(scene.spawnsSent().empty());
}

/**
 * Given a spawner and a player
 * When the first pause has just passed
 * Then one Bydo is in the world and the player is sent one EntitySpawned for
 * it, past the right edge
 */
TEST(WaveSpawner, ABydoAppearsOffScreenWhenTheFirstPauseEnds) {
  WaveSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  WaveSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  WaveSpawnerScene::runTicks(spawner, TICKS_TO_THE_FIRST_BYDO);

  EXPECT_EQ(scene.world().entityCount(), 1U);
  const std::vector<EntitySpawned> spawns = scene.spawnsSent();
  ASSERT_EQ(spawns.size(), 1U);
  EXPECT_EQ(spawns.front().entityType,
            shippedEntitySheets().named(DRIFTER_SHEET).number);
  EXPECT_GT(spawns.front().x, PLAYFIELD_WIDTH);
  EXPECT_GE(spawns.front().y, ENEMY_ROW_MARGIN);
  EXPECT_LE(spawns.front().y, PLAY_AREA_HEIGHT - ENEMY_ROW_MARGIN);
}

/**
 * Given a spawner and two players
 * When the second Bydo of the first wave has appeared
 * Then two Bydos are in the world and each player was sent both, at the same
 * heights
 */
TEST(WaveSpawner, EveryPlayerIsToldAboutEveryBydo) {
  WaveSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  scene.connect(SECOND_PLAYER);
  WaveSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  WaveSpawnerScene::runTicks(spawner, TICKS_TO_THE_SECOND_BYDO);

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
TEST(WaveSpawner, TheSeedDecidesTheHeights) {
  const std::vector<float> reference =
      WaveSpawnerScene::heightsOfTheFirstWave(SPAWNER_SEED);
  ASSERT_EQ(reference.size(), TEST_WAVE_CURVE.firstWave.entityCount);

  EXPECT_EQ(WaveSpawnerScene::heightsOfTheFirstWave(SPAWNER_SEED), reference);
  EXPECT_NE(WaveSpawnerScene::heightsOfTheFirstWave(OTHER_SPAWNER_SEED),
            reference);
}

/**
 * Given the Bydos of a first wave, announced from one seed
 * When their heights are compared
 * Then they are not all the same, so the height is not a fixed row
 */
TEST(WaveSpawner, HeightsVaryFromOneBydoToTheNext) {
  const std::vector<float> heights =
      WaveSpawnerScene::heightsOfTheFirstWave(SPAWNER_SEED);

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
TEST(WaveSpawner, ABydoAppearsEvenWithNobodyToTell) {
  WaveSpawnerScene scene;
  WaveSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  WaveSpawnerScene::runTicks(spawner, TICKS_TO_THE_FIRST_BYDO);

  EXPECT_EQ(scene.world().entityCount(), 1U);
  EXPECT_TRUE(scene.spawnsSent().empty());
}

/**
 * Given a log file as the only sink
 * When a spawner is started from a seed
 * Then the log holds a line with that seed, so the case can be replayed
 */
TEST(WaveSpawner, TheSeedIsLogged) {
  Logger::setLevel(LogLevel::Info);
  const JournalFile journal;
  Logger::setOutput({.toStderr = false, .filePath = journal.path()});
  WaveSpawnerScene scene;

  const WaveSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

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
TEST(WaveSpawner, TheStartOfAWaveIsLogged) {
  Logger::setLevel(LogLevel::Info);
  const JournalFile journal;
  Logger::setOutput({.toStderr = false, .filePath = journal.path()});
  WaveSpawnerScene scene;
  WaveSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  WaveSpawnerScene::runTicks(spawner, 1);

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
TEST(WaveSpawner, TheBydosTakeTheKindsOfTheirPlanInTurn) {
  WaveSpawnerScene scene;
  WaveSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  WaveSpawnerScene::runTicks(spawner, TICKS_TO_THE_WHOLE_FIRST_WAVE);

  const std::vector<std::string_view> behaviours = scene.behavioursOfTheBydos();
  EXPECT_EQ(std::ranges::count(behaviours, STRAIGHT_BEHAVIOUR_NAME), 2);
  EXPECT_EQ(std::ranges::count(behaviours, WEAVE_BEHAVIOUR_NAME), 1);
}

/**
 * Given a spawner whose plan names the Weaver first
 * When the first Bydo appears
 * Then it weaves: the cycle starts with the first kind the plan names
 */
TEST(WaveSpawner, TheCycleStartsWithTheFirstKindItNames) {
  WaveSpawnerScene scene;
  WaveSpawner spawner =
      scene.makeSpawnerWith(SPAWNER_SEED, wavePlanOf(WEAVER_FIRST_CYCLE));

  WaveSpawnerScene::runTicks(spawner, TICKS_TO_THE_FIRST_BYDO);

  EXPECT_EQ(scene.behavioursOfTheBydos(), std::vector{WEAVE_BEHAVIOUR_NAME});
}

/**
 * Given a spawner whose plan takes the Drifter then the Weaver in turn, its
 * first wave of three Bydos gone from the world
 * When a pause passes and the first Bydo of the second wave appears
 * Then it weaves: it is the fourth of the cycle, which carries on from one
 * wave to the next
 */
TEST(WaveSpawner, TheCycleCarriesOnIntoTheNextWave) {
  WaveSpawnerScene scene;
  WaveSpawner spawner = scene.makeSpawner(SPAWNER_SEED);
  WaveSpawnerScene::runTicks(spawner, TICKS_TO_THE_WHOLE_FIRST_WAVE);
  scene.clearTheSky();

  WaveSpawnerScene::runTicks(spawner, TICKS_OF_A_PAUSE);

  EXPECT_EQ(scene.behavioursOfTheBydos(), std::vector{WEAVE_BEHAVIOUR_NAME});
}

/**
 * Given a spawner whose first wave appeared, its Bydos still in the world
 * When twenty more seconds pass
 * Then no other Bydo appears: the next wave waits for the world to hold no
 * Bydo
 */
TEST(WaveSpawner, TheNextWaveWaitsForTheBydosLeftInTheWorld) {
  WaveSpawnerScene scene;
  WaveSpawner spawner = scene.makeSpawner(SPAWNER_SEED);
  WaveSpawnerScene::runTicks(spawner, TICKS_TO_THE_WHOLE_FIRST_WAVE);
  ASSERT_EQ(scene.world().entityCount(), TEST_WAVE_CURVE.firstWave.entityCount);

  WaveSpawnerScene::runTicks(spawner, TWENTY_SECONDS_OF_TICKS);

  EXPECT_EQ(scene.world().entityCount(), TEST_WAVE_CURVE.firstWave.entityCount);
}

/**
 * Given a spawner on a level of one wave, whose first wave appeared
 * When the Bydos are killed, then a tick passes
 * Then the last wave is finished, and it stays so while a minute passes with
 * no Bydo appearing
 */
TEST(WaveSpawner, TheLastWaveIsFinishedOnceItsBydosAreGone) {
  WaveSpawnerScene scene;
  WaveSpawner spawner =
      scene.makeSpawnerWith(SPAWNER_SEED, oneWaveLevelWavePlan());
  WaveSpawnerScene::runTicks(spawner, TICKS_TO_THE_WHOLE_FIRST_WAVE);
  ASSERT_FALSE(spawner.lastWaveFinished());
  scene.clearTheSky();

  WaveSpawnerScene::runTicks(spawner, 1);

  EXPECT_TRUE(spawner.lastWaveFinished());
  WaveSpawnerScene::runTicks(spawner, TWENTY_SECONDS_OF_TICKS);
  EXPECT_EQ(scene.world().entityCount(), 0U);
}

/**
 * Given a spawner on a level of one wave, whose first wave appeared
 * When twenty seconds pass with its Bydos still in the world
 * Then the last wave is not finished
 */
TEST(WaveSpawner, TheLastWaveIsNotFinishedWhileABydoIsInTheWorld) {
  WaveSpawnerScene scene;
  WaveSpawner spawner =
      scene.makeSpawnerWith(SPAWNER_SEED, oneWaveLevelWavePlan());

  WaveSpawnerScene::runTicks(spawner, TICKS_TO_THE_WHOLE_FIRST_WAVE);
  WaveSpawnerScene::runTicks(spawner, TWENTY_SECONDS_OF_TICKS);

  EXPECT_FALSE(spawner.lastWaveFinished());
}

/**
 * Given a wave plan that names no enemy
 * When a spawner is built on it
 * Then an EmptyEnemyCycleException is thrown
 */
TEST(WaveSpawner, RefusesAPlanWithoutEnemies) {
  WaveSpawnerScene scene;

  EXPECT_THROW(
      static_cast<void>(scene.makeSpawnerWith(SPAWNER_SEED, wavePlanOf({}))),
      EmptyEnemyCycleException);
}

/**
 * Given a wave plan naming a sheet the server does not have
 * When a spawner is built with it
 * Then it is refused with an UnknownEntityTypeException
 */
TEST(WaveSpawner, RefusesAPlanNamingNoSheet) {
  WaveSpawnerScene scene;

  EXPECT_THROW(static_cast<void>(scene.makeSpawnerWith(
                   SPAWNER_SEED, wavePlanOf(NO_SHEET_CYCLE))),
               UnknownEntityTypeException);
}

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
#include "SpawningConstants.hpp"
#include "WorldState.hpp"

using rtype::game::BYDO_ROW_MARGIN;
using rtype::game::PatternShape;
using rtype::game::PLAY_AREA_HEIGHT;
using rtype::game::PLAYFIELD_WIDTH;
using rtype::logging::Logger;
using rtype::logging::LogLevel;
using rtype::network::EntitySpawned;
using rtype::network::WireEntityType;
using rtype::server::BydoSpawner;
using rtype::server::testing::BydoSpawnerScene;
using rtype::server::testing::FIRST_PLAYER;
using rtype::server::testing::OTHER_SPAWNER_SEED;
using rtype::server::testing::SECOND_PLAYER;
using rtype::server::testing::SOME_BYDO_INTERVALS;
using rtype::server::testing::SPAWNER_SEED;
using rtype::server::testing::TICKS_PER_BYDO_INTERVAL;

/**
 * Given a spawner and a player
 * When one tick less than the interval has passed
 * Then no Bydo has appeared and nothing was sent
 */
TEST(BydoSpawner, NothingAppearsBeforeTheInterval) {
  BydoSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  BydoSpawnerScene::runTicks(spawner, TICKS_PER_BYDO_INTERVAL - 1);

  EXPECT_EQ(scene.world().entityCount(), 0U);
  EXPECT_TRUE(scene.spawnsSent().empty());
}

/**
 * Given a spawner and a player
 * When the interval has just passed
 * Then one Bydo is in the world and the player is sent one EntitySpawned for
 * it, past the right edge
 */
TEST(BydoSpawner, ABydoAppearsOffScreenWhenTheIntervalPasses) {
  BydoSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  BydoSpawnerScene::runTicks(spawner, TICKS_PER_BYDO_INTERVAL);

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
 * When two intervals have passed
 * Then two Bydos are in the world and each player was sent both, at the same
 * heights
 */
TEST(BydoSpawner, EveryPlayerIsToldAboutEveryBydo) {
  BydoSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  scene.connect(SECOND_PLAYER);
  BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  BydoSpawnerScene::runTicks(spawner, 2 * TICKS_PER_BYDO_INTERVAL);

  EXPECT_EQ(scene.world().entityCount(), 2U);
  const std::vector<float> first = scene.heightsSentTo(FIRST_PLAYER);
  EXPECT_EQ(first.size(), 2U);
  EXPECT_EQ(first, scene.heightsSentTo(SECOND_PLAYER));
}

/**
 * Given two spawners started from the same seed
 * When the same intervals pass for both
 * Then they announce the same heights in the same order, and a spawner started
 * from another seed announces other heights
 */
TEST(BydoSpawner, TheSeedDecidesTheHeights) {
  const std::vector<float> reference = BydoSpawnerScene::heightsWithin(
      SPAWNER_SEED, SOME_BYDO_INTERVALS * TICKS_PER_BYDO_INTERVAL);
  ASSERT_EQ(reference.size(), SOME_BYDO_INTERVALS);

  EXPECT_EQ(BydoSpawnerScene::heightsWithin(
                SPAWNER_SEED, SOME_BYDO_INTERVALS * TICKS_PER_BYDO_INTERVAL),
            reference);
  EXPECT_NE(
      BydoSpawnerScene::heightsWithin(
          OTHER_SPAWNER_SEED, SOME_BYDO_INTERVALS * TICKS_PER_BYDO_INTERVAL),
      reference);
}

/**
 * Given twenty Bydos announced from one seed
 * When their heights are compared
 * Then they are not all the same, so the height is not a fixed row
 */
TEST(BydoSpawner, HeightsVaryFromOneBydoToTheNext) {
  const std::vector<float> heights = BydoSpawnerScene::heightsWithin(
      SPAWNER_SEED, SOME_BYDO_INTERVALS * TICKS_PER_BYDO_INTERVAL);

  EXPECT_GT(std::ranges::count_if(
                heights,
                [&heights](float height) { return height != heights.front(); }),
            0);
}

/**
 * Given no player connected
 * When an interval passes
 * Then the Bydo is in the world and nothing is sent
 */
TEST(BydoSpawner, ABydoAppearsEvenWithNobodyToTell) {
  BydoSpawnerScene scene;
  BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  BydoSpawnerScene::runTicks(spawner, TICKS_PER_BYDO_INTERVAL);

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
 * Given a spawner and a player
 * When three intervals have passed
 * Then two Bydos go straight and one follows a wave: the kinds alternate,
 * starting with the one that goes straight
 */
TEST(BydoSpawner, KindsAlternateStartingWithTheOneThatGoesStraight) {
  BydoSpawnerScene scene;
  scene.connect(FIRST_PLAYER);
  BydoSpawner spawner = scene.makeSpawner(SPAWNER_SEED);

  BydoSpawnerScene::runTicks(spawner, 3 * TICKS_PER_BYDO_INTERVAL);

  const std::vector<PatternShape> shapes = scene.patternsOfTheBydos();
  EXPECT_EQ(std::ranges::count(shapes, PatternShape::Straight), 2);
  EXPECT_EQ(std::ranges::count(shapes, PatternShape::Wave), 1);
}

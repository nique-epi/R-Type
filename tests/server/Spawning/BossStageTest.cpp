#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "BossArrival.hpp"
#include "BossStageConstants.hpp"
#include "BossStageFixture.hpp"
#include "BossStageTestConstants.hpp"
#include "EntitySpawned.hpp"
#include "Position.hpp"

using rtype::game::BossArrival;
using rtype::game::Position;
using rtype::network::EntitySpawned;
using rtype::server::BOSS_ARRIVAL_WARNING_MILLISECONDS;
using rtype::server::BOSS_ENTRY_POSITION;
using rtype::server::testing::A_FEW_STAGE_TICKS;
using rtype::server::testing::BossStageFixture;
using rtype::server::testing::FIRST_STAGE_PLAYER;
using rtype::server::testing::LONGER_THAN_THE_WARNING;
using rtype::server::testing::SECOND_STAGE_PLAYER;
using rtype::server::testing::STAGE_PLAYER_COUNT;
using rtype::server::testing::TICKS_TO_ANNOUNCE_THE_BOSS;
using rtype::server::testing::TICKS_TO_THE_BOSS;
using rtype::server::testing::TICKS_TO_THE_LAST_TICK_OF_THE_WARNING;
using ::testing::Each;
using ::testing::Field;

/**
 * Given a stage whose level ends with the warlord
 * When ticks run while the waves go on
 * Then no arrival is announced and no boss appears
 */
TEST_F(BossStageFixture, AnnouncesNothingWhileTheWavesGoOn) {
  runTicks(A_FEW_STAGE_TICKS, false);

  EXPECT_TRUE(arrivals().empty());
  EXPECT_TRUE(bosses().empty());
}

/**
 * Given a stage whose level ends with the warlord
 * When the first tick that finds the last wave over runs
 * Then the arrival of the warlord is announced, with its warning
 */
TEST_F(BossStageFixture, AnnouncesTheBossOnceTheLastWaveIsOver) {
  runTicks(TICKS_TO_ANNOUNCE_THE_BOSS, true);

  const std::vector<BossArrival> expected{
      {.typeNumber = warlordNumber(),
       .warningMilliseconds = BOSS_ARRIVAL_WARNING_MILLISECONDS}};
  EXPECT_EQ(arrivals(), expected);
}

/**
 * Given a stage that announced the warlord
 * When the ticks of the warning run
 * Then the warlord has not appeared yet
 */
TEST_F(BossStageFixture, KeepsTheBossOutDuringTheWarning) {
  runTicks(TICKS_TO_THE_LAST_TICK_OF_THE_WARNING, true);

  EXPECT_TRUE(bosses().empty());
}

/**
 * Given a stage that announced the warlord
 * When the tick after the warning runs
 * Then the warlord appears, half its frame past the right edge
 */
TEST_F(BossStageFixture, BringsTheBossInAfterTheWarning) {
  runTicks(TICKS_TO_THE_BOSS, true);

  const std::vector<Position> found = bosses();
  ASSERT_EQ(found.size(), 1U);
  EXPECT_FLOAT_EQ(found.front().x, BOSS_ENTRY_POSITION.x);
  EXPECT_FLOAT_EQ(found.front().y, BOSS_ENTRY_POSITION.y);
}

/**
 * Given two connected players and a stage that announced the warlord
 * When the warlord appears
 * Then each player is sent one EntitySpawned of the warlord
 */
TEST_F(BossStageFixture, TellsEveryPlayerAboutTheBoss) {
  connectPlayers(STAGE_PLAYER_COUNT);

  runTicks(TICKS_TO_THE_BOSS, true);

  for (const auto player : {FIRST_STAGE_PLAYER, SECOND_STAGE_PLAYER}) {
    const std::vector<EntitySpawned> spawns = spawnsSentTo(player);
    EXPECT_EQ(spawns.size(), 1U);
    EXPECT_THAT(spawns,
                Each(Field(&EntitySpawned::entityType, warlordNumber())));
  }
}

/**
 * Given the warlord in play
 * When ticks run
 * Then the level is not cleared
 */
TEST_F(BossStageFixture, IsNotClearedWhileTheBossLives) {
  runTicks(TICKS_TO_THE_BOSS, true);

  runTicks(A_FEW_STAGE_TICKS, true);

  EXPECT_FALSE(cleared());
}

/**
 * Given the warlord in play, then killed
 * When one tick runs
 * Then the level is cleared: no enemy is left
 */
TEST_F(BossStageFixture, IsClearedOnceNoEnemyIsLeft) {
  runTicks(TICKS_TO_THE_BOSS, true);
  destroyTheBosses();

  runTicks(1, true);

  EXPECT_TRUE(cleared());
}

/**
 * Given the warlord in play with a minion, then the warlord killed
 * When one tick runs
 * Then the level is not cleared: the minion is an enemy left
 */
TEST_F(BossStageFixture, IsClearedOnlyOnceAMinionIsGoneToo) {
  runTicks(TICKS_TO_THE_BOSS, true);
  addMinion();
  destroyTheBosses();

  runTicks(1, true);

  EXPECT_FALSE(cleared());
}

/**
 * Given a stage whose level ends with its last wave, with no boss
 * When the first tick that finds the last wave over runs
 * Then the level is cleared on that tick
 */
TEST_F(BossStageFixture, IsClearedOnTheTickTheLastWaveIsOver) {
  buildStage(std::nullopt);

  runTicks(1, true);

  EXPECT_TRUE(cleared());
}

/**
 * Given a stage whose level ends with its last wave, with no boss
 * When ticks longer than the warning run with the last wave over
 * Then no arrival is announced and no boss appears
 */
TEST_F(BossStageFixture, AnnouncesNoBoss) {
  buildStage(std::nullopt);

  runTicks(LONGER_THAN_THE_WARNING, true);

  EXPECT_TRUE(arrivals().empty());
  EXPECT_TRUE(bosses().empty());
}

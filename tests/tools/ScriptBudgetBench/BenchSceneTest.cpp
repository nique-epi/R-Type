#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>
#include "BenchContentFolders.hpp"
#include "BenchPopulation.hpp"
#include "BenchScene.hpp"
#include "CollisionSystemConstants.hpp"
#include "ScriptBudgetBenchConstants.hpp"
#include "ScriptBudgetBenchTestConstants.hpp"
#include "TickSample.hpp"
#include "TimeConstants.hpp"

using rtype::engine::Duration;
using rtype::game::COLLISION_CELL_SIDE;
using rtype::tools::BENCH_SHIPS;
using rtype::tools::benchContentFolders;
using rtype::tools::BenchPopulation;
using rtype::tools::BenchScene;
using rtype::tools::DEFAULT_BULLETS;
using rtype::tools::TickSample;
using rtype::tools::testing::FEW_BODIES;
using rtype::tools::testing::FEW_BULLETS;
using rtype::tools::testing::FEW_SCRIPTED_BODIES;
using rtype::tools::testing::FEW_WAITING_BODIES;
using rtype::tools::testing::MEASURED_CELL_SIDES;
using rtype::tools::testing::MOST_BULLETS_ON_THE_SHIPS;
using rtype::tools::testing::TICKS_TO_CROSS_THE_PLAYFIELD;

/**
 * Given a scene whose bodies leave the playfield
 * When every bullet had the time to cross it
 * Then the population is what it was, and the script never failed
 */
TEST(BenchScene, ReplacesTheBodiesThatLeave) {
  BenchScene scene{FEW_BODIES, COLLISION_CELL_SIDE, benchContentFolders()};

  for (std::size_t tick = 0; tick < TICKS_TO_CROSS_THE_PLAYFIELD; ++tick) {
    (void)scene.runTick();
  }

  EXPECT_EQ(scene.bodiesInPlay(),
            FEW_SCRIPTED_BODIES + FEW_BULLETS + BENCH_SHIPS);
  EXPECT_EQ(scene.scriptFailure(), "");
}

/**
 * Given a scene whose scripted bodies wait a tick at a time
 * When every bullet had the time to cross the playfield
 * Then the population is what it was, and the script never failed
 */
TEST(BenchScene, ReplacesTheWaitingBodiesThatLeave) {
  BenchScene scene{FEW_WAITING_BODIES, COLLISION_CELL_SIDE,
                   benchContentFolders()};

  for (std::size_t tick = 0; tick < TICKS_TO_CROSS_THE_PLAYFIELD; ++tick) {
    (void)scene.runTick();
  }

  EXPECT_EQ(scene.bodiesInPlay(),
            FEW_SCRIPTED_BODIES + FEW_BULLETS + BENCH_SHIPS);
  EXPECT_EQ(scene.scriptFailure(), "");
}

/**
 * Given a scene with scripted bodies
 * When a tick runs
 * Then the time of its scripts is counted, as a part of the tick
 */
TEST(BenchScene, CountsTheScriptsAsAPartOfTheTick) {
  BenchScene scene{FEW_BODIES, COLLISION_CELL_SIDE, benchContentFolders()};

  const TickSample sample = scene.runTick();

  EXPECT_GT(sample.scripts, Duration::zero());
  EXPECT_LT(sample.scripts, sample.whole);
}

/**
 * Given a scene without scripted body
 * When a tick runs
 * Then no time is counted for the scripts
 */
TEST(BenchScene, CountsNoScriptTimeWithoutScriptedBodies) {
  BenchScene scene{BenchPopulation{.scriptedBodies = 0, .bullets = FEW_BULLETS},
                   COLLISION_CELL_SIDE, benchContentFolders()};

  const TickSample sample = scene.runTick();

  EXPECT_EQ(sample.scripts, Duration::zero());
}

/**
 * Given a scene with scripted bodies and bullets
 * When a tick runs
 * Then the time of CollisionSystem is counted, as a part of the tick
 */
TEST(BenchScene, CountsTheCollisionsAsAPartOfTheTick) {
  BenchScene scene{FEW_BODIES, COLLISION_CELL_SIDE, benchContentFolders()};

  const TickSample sample = scene.runTick();

  EXPECT_GT(sample.collisions, Duration::zero());
  EXPECT_LT(sample.collisions, sample.whole);
}

/**
 * Given three scenes alike but for the side of their collision cells
 * When every bullet had the time to cross the playfield
 * Then the three found the same collisions, and found some
 */
TEST(BenchScene, FindsTheSameCollisionsWhateverTheCellSide) {
  std::vector<std::size_t> found;
  for (const float cellSide : MEASURED_CELL_SIDES) {
    BenchScene scene{FEW_BODIES, cellSide, benchContentFolders()};
    std::size_t collisions = 0;
    for (std::size_t tick = 0; tick < TICKS_TO_CROSS_THE_PLAYFIELD; ++tick) {
      collisions += scene.runTick().collisionsFound;
    }
    found.push_back(collisions);
  }

  EXPECT_GT(found.front(), 0U);
  EXPECT_EQ(std::ranges::count(found, found.front()), std::ssize(found));
}

/**
 * Given a scene of the ships and a thousand bullets, with no scripted body
 * When a tick runs
 * Then no bullet touches another: the only collisions are with the ships
 */
TEST(BenchScene, KeepsTheBulletsApartFromEachOther) {
  BenchScene scene{
      BenchPopulation{.scriptedBodies = 0, .bullets = DEFAULT_BULLETS},
      COLLISION_CELL_SIDE, benchContentFolders()};

  const TickSample sample = scene.runTick();

  EXPECT_LE(sample.collisionsFound, MOST_BULLETS_ON_THE_SHIPS);
}

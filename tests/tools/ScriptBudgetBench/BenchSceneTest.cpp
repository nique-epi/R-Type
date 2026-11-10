#include <gtest/gtest.h>
#include <cstddef>
#include "BenchContentFolders.hpp"
#include "BenchPopulation.hpp"
#include "BenchScene.hpp"
#include "ScriptBudgetBenchConstants.hpp"
#include "ScriptBudgetBenchTestConstants.hpp"
#include "TickSample.hpp"
#include "TimeConstants.hpp"

using rtype::engine::Duration;
using rtype::tools::BENCH_SHIPS;
using rtype::tools::benchContentFolders;
using rtype::tools::BenchPopulation;
using rtype::tools::BenchScene;
using rtype::tools::TickSample;
using rtype::tools::testing::FEW_BODIES;
using rtype::tools::testing::FEW_BULLETS;
using rtype::tools::testing::FEW_SCRIPTED_BODIES;
using rtype::tools::testing::FEW_WAITING_BODIES;
using rtype::tools::testing::TICKS_TO_CROSS_THE_PLAYFIELD;

/**
 * Given a scene whose bodies leave the playfield
 * When every bullet had the time to cross it
 * Then the population is what it was, and the script never failed
 */
TEST(BenchScene, ReplacesTheBodiesThatLeave) {
  BenchScene scene{FEW_BODIES, benchContentFolders()};

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
  BenchScene scene{FEW_WAITING_BODIES, benchContentFolders()};

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
  BenchScene scene{FEW_BODIES, benchContentFolders()};

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
                   benchContentFolders()};

  const TickSample sample = scene.runTick();

  EXPECT_EQ(sample.scripts, Duration::zero());
}

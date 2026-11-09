#include <gtest/gtest.h>
#include <array>
#include "BenchLaunch.hpp"
#include "BenchLaunchOptions.hpp"
#include "EngineException.hpp"
#include "ScriptBudgetBenchConstants.hpp"
#include "ScriptBudgetBenchTestConstants.hpp"

using rtype::engine::InvalidLaunchOptionValueException;
using rtype::tools::BenchLaunch;
using rtype::tools::BULLETS_OPTION;
using rtype::tools::DEFAULT_BENCH_TICKS;
using rtype::tools::readBenchLaunch;
using rtype::tools::SCRIPTED_BODIES_OPTION;
using rtype::tools::TICKS_OPTION;
using rtype::tools::testing::NO_TICK;
using rtype::tools::testing::NOT_A_NUMBER;
using rtype::tools::testing::SEVEN;
using rtype::tools::testing::SEVEN_BODIES;

/**
 * Given no argument
 * When the launch is read
 * Then the bench measures the load it was written for
 */
TEST(BenchLaunchOptions, MeasuresThreeHundredBodiesAndAThousandBullets) {
  const BenchLaunch launch = readBenchLaunch({});

  EXPECT_EQ(launch.population.scriptedBodies, 300U);
  EXPECT_EQ(launch.population.bullets, 1000U);
  EXPECT_EQ(launch.ticks, DEFAULT_BENCH_TICKS);
}

/**
 * Given a count of scripted bodies
 * When the launch is read
 * Then the bullets keep their default
 */
TEST(BenchLaunchOptions, ReadsEachCountOnItsOwn) {
  const std::array arguments{SCRIPTED_BODIES_OPTION, SEVEN_BODIES};

  const BenchLaunch launch = readBenchLaunch(arguments);

  EXPECT_EQ(launch.population.scriptedBodies, SEVEN);
  EXPECT_EQ(launch.population.bullets, 1000U);
}

/**
 * Given no tick to run
 * When the launch is read
 * Then it is refused, a run of no tick having no figure
 */
TEST(BenchLaunchOptions, RefusesARunOfNoTick) {
  const std::array arguments{TICKS_OPTION, NO_TICK};

  EXPECT_THROW((void)readBenchLaunch(arguments),
               InvalidLaunchOptionValueException);
}

/**
 * Given a count that is no number
 * When the launch is read
 * Then it is refused
 */
TEST(BenchLaunchOptions, RefusesACountThatIsNoNumber) {
  const std::array arguments{BULLETS_OPTION, NOT_A_NUMBER};

  EXPECT_THROW((void)readBenchLaunch(arguments),
               InvalidLaunchOptionValueException);
}

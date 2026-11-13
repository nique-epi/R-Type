#include <gtest/gtest.h>
#include <optional>
#include "MachineCounterReadings.hpp"
#include "MachineLoad.hpp"
#include "MachineLoadTestConstants.hpp"
#include "ServerMeasures.hpp"

using rtype::network::ServerMeasures;
using rtype::server::MachineCounterReadings;
using rtype::server::withMachineLoad;
using rtype::server::testing::ALL_IDLE_COUNTERS;
using rtype::server::testing::EARLIER_COUNTERS;
using rtype::server::testing::EIGHT_GIGABYTES;
using rtype::server::testing::FLAT_OUT_PERCENT;
using rtype::server::testing::GAMES_MEDIAN_TICK_MILLISECONDS;
using rtype::server::testing::IDLE_PERCENT;
using rtype::server::testing::IDLE_TIME_BACK_COUNTERS;
using rtype::server::testing::LATEST_COUNTERS;
using rtype::server::testing::MORE_IDLE_THAN_TIME_COUNTERS;
using rtype::server::testing::MORE_USED_THAN_HELD_COUNTERS;
using rtype::server::testing::NO_MEMORY_COUNTERS;
using rtype::server::testing::NONE_IDLE_COUNTERS;
using rtype::server::testing::SIX_GIGABYTES;
using rtype::server::testing::THREE_QUARTERS_BUSY_PERCENT;

/**
 * Given two readings between which a quarter of the processor time was idle
 * When the load is added to measures
 * Then three quarters of the processors are busy, and the memory is that of
 * the latest reading
 */
TEST(MachineLoad, TellsTheBusyShareBetweenTwoReadingsAndTheLatestMemory) {
  const ServerMeasures measures = withMachineLoad(
      ServerMeasures{}, MachineCounterReadings{.earlier = EARLIER_COUNTERS,
                                               .latest = LATEST_COUNTERS});

  EXPECT_EQ(measures.cpuPercent, THREE_QUARTERS_BUSY_PERCENT);
  EXPECT_EQ(measures.usedMemoryBytes, SIX_GIGABYTES);
  EXPECT_EQ(measures.totalMemoryBytes, EIGHT_GIGABYTES);
}

/**
 * Given measures of the games with a median tick
 * When the load is added to them
 * Then the median tick is still there
 */
TEST(MachineLoad, KeepsTheMeasuresOfTheGames) {
  ServerMeasures games{};
  games.tickP50Milliseconds = GAMES_MEDIAN_TICK_MILLISECONDS;

  const ServerMeasures measures =
      withMachineLoad(games, MachineCounterReadings{.earlier = EARLIER_COUNTERS,
                                                    .latest = LATEST_COUNTERS});

  EXPECT_EQ(measures.tickP50Milliseconds, GAMES_MEDIAN_TICK_MILLISECONDS);
}

/**
 * Given a first reading, with none before it
 * When the load is added to measures
 * Then the memory is told and the share of busy processors is not, since it
 * needs two readings
 */
TEST(MachineLoad, TellsNoBusyShareAtTheFirstReading) {
  const ServerMeasures measures = withMachineLoad(
      ServerMeasures{}, MachineCounterReadings{.earlier = std::nullopt,
                                               .latest = LATEST_COUNTERS});

  EXPECT_FALSE(measures.cpuPercent.has_value());
  EXPECT_EQ(measures.usedMemoryBytes, SIX_GIGABYTES);
}

/**
 * Given a system that gave no counters this time
 * When the load is added to measures
 * Then neither the processors nor the memory is told
 */
TEST(MachineLoad, TellsNothingWithoutTheLatestReading) {
  const ServerMeasures measures = withMachineLoad(
      ServerMeasures{}, MachineCounterReadings{.earlier = EARLIER_COUNTERS,
                                               .latest = std::nullopt});

  EXPECT_EQ(measures, ServerMeasures{});
}

/**
 * Given two readings between which every unit of processor time was idle
 * When the load is added to measures
 * Then the busy share is 0 %, which is a measure and not an absence
 */
TEST(MachineLoad, TellsZeroForAnIdleMachine) {
  const ServerMeasures measures = withMachineLoad(
      ServerMeasures{}, MachineCounterReadings{.earlier = LATEST_COUNTERS,
                                               .latest = ALL_IDLE_COUNTERS});

  EXPECT_EQ(measures.cpuPercent, IDLE_PERCENT);
}

/**
 * Given two readings between which no unit of processor time was idle
 * When the load is added to measures
 * Then the busy share is 100 %
 */
TEST(MachineLoad, TellsAHundredForAMachineRunningFlatOut) {
  const ServerMeasures measures = withMachineLoad(
      ServerMeasures{}, MachineCounterReadings{.earlier = LATEST_COUNTERS,
                                               .latest = NONE_IDLE_COUNTERS});

  EXPECT_EQ(measures.cpuPercent, FLAT_OUT_PERCENT);
}

/**
 * Given the same reading twice, so no processor time passed
 * When the load is added to measures
 * Then no busy share is told
 */
TEST(MachineLoad, TellsNoBusyShareWhenNoTimePassed) {
  const ServerMeasures measures = withMachineLoad(
      ServerMeasures{}, MachineCounterReadings{.earlier = LATEST_COUNTERS,
                                               .latest = LATEST_COUNTERS});

  EXPECT_FALSE(measures.cpuPercent.has_value());
}

/**
 * Given a latest reading whose processor time is behind the earlier one, as
 * after a restart of the machine
 * When the load is added to measures
 * Then no busy share is told
 */
TEST(MachineLoad, TellsNoBusyShareWhenTheTimeWentBack) {
  const ServerMeasures measures = withMachineLoad(
      ServerMeasures{}, MachineCounterReadings{.earlier = LATEST_COUNTERS,
                                               .latest = EARLIER_COUNTERS});

  EXPECT_FALSE(measures.cpuPercent.has_value());
}

/**
 * Given a latest reading whose idle time is one unit behind the earlier one
 * When the load is added to measures
 * Then no busy share is told
 */
TEST(MachineLoad, TellsNoBusyShareWhenTheIdleTimeWentBack) {
  const ServerMeasures measures = withMachineLoad(
      ServerMeasures{},
      MachineCounterReadings{.earlier = LATEST_COUNTERS,
                             .latest = IDLE_TIME_BACK_COUNTERS});

  EXPECT_FALSE(measures.cpuPercent.has_value());
}

/**
 * Given a latest reading that counted one more idle unit than units passed
 * When the load is added to measures
 * Then no busy share is told
 */
TEST(MachineLoad, TellsNoBusyShareWhenMoreWasIdleThanPassed) {
  const ServerMeasures measures = withMachineLoad(
      ServerMeasures{},
      MachineCounterReadings{.earlier = LATEST_COUNTERS,
                             .latest = MORE_IDLE_THAN_TIME_COUNTERS});

  EXPECT_FALSE(measures.cpuPercent.has_value());
}

/**
 * Given a reading of a machine that says it holds no memory
 * When the load is added to measures
 * Then no memory is told, so nobody divides by it
 */
TEST(MachineLoad, TellsNoMemoryOfAMachineThatHoldsNone) {
  const ServerMeasures measures = withMachineLoad(
      ServerMeasures{}, MachineCounterReadings{.earlier = std::nullopt,
                                               .latest = NO_MEMORY_COUNTERS});

  EXPECT_FALSE(measures.usedMemoryBytes.has_value());
  EXPECT_FALSE(measures.totalMemoryBytes.has_value());
}

/**
 * Given a reading that uses one byte more than the machine holds
 * When the load is added to measures
 * Then no memory is told
 */
TEST(MachineLoad, TellsNoMemoryWhenMoreIsUsedThanHeld) {
  const ServerMeasures measures = withMachineLoad(
      ServerMeasures{},
      MachineCounterReadings{.earlier = std::nullopt,
                             .latest = MORE_USED_THAN_HELD_COUNTERS});

  EXPECT_FALSE(measures.usedMemoryBytes.has_value());
  EXPECT_FALSE(measures.totalMemoryBytes.has_value());
}

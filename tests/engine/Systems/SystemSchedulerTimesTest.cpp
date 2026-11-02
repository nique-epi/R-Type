#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "ClockAdvancingSystem.hpp"
#include "ComponentRegistry.hpp"
#include "EntityRegistry.hpp"
#include "SimulatedClock.hpp"
#include "SystemScheduler.hpp"
#include "SystemSchedulerTimesTestConstants.hpp"
#include "SystemTime.hpp"
#include "TimeConstants.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::EntityRegistry;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::engine::SystemScheduler;
using rtype::engine::SystemTime;

/**
 * Given a scheduler that holds no system
 * When its times are taken
 * Then there is none
 */
TEST(SystemSchedulerTimes, ASchedulerWithoutSystemsHasNoTimes) {
  SystemScheduler scheduler;

  EXPECT_TRUE(scheduler.takeSystemTimes().empty());
}

/**
 * Given two named systems of known times, measured by a clock
 * When the scheduler runs once
 * Then each system has its own time under its own name, in the order they were
 * added
 */
TEST(SystemSchedulerTimes, GivesTheTimeOfEachSystemUnderItsName) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  SimulatedClock clock;
  SystemScheduler scheduler;
  scheduler.add(
      std::make_unique<ClockAdvancingSystem>(clock, FIRST_SYSTEM_TIME),
      FIRST_SYSTEM_NAME);
  scheduler.add(
      std::make_unique<ClockAdvancingSystem>(clock, SECOND_SYSTEM_TIME),
      SECOND_SYSTEM_NAME);
  scheduler.measureWith(&clock);

  scheduler.run(components, SIMULATION_TICK_DURATION);

  const std::vector<SystemTime> times = scheduler.takeSystemTimes();
  ASSERT_EQ(times.size(), 2U);
  EXPECT_EQ(times[0].name, FIRST_SYSTEM_NAME);
  EXPECT_EQ(times[0].total, FIRST_SYSTEM_TIME);
  EXPECT_EQ(times[1].name, SECOND_SYSTEM_NAME);
  EXPECT_EQ(times[1].total, SECOND_SYSTEM_TIME);
}

/**
 * Given a system of a known time, measured by a clock
 * When the scheduler runs twice
 * Then its time is the time of both runs
 */
TEST(SystemSchedulerTimes, AddsUpTheTimesOfSeveralRuns) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  SimulatedClock clock;
  SystemScheduler scheduler;
  scheduler.add(
      std::make_unique<ClockAdvancingSystem>(clock, FIRST_SYSTEM_TIME),
      FIRST_SYSTEM_NAME);
  scheduler.measureWith(&clock);

  scheduler.run(components, SIMULATION_TICK_DURATION);
  scheduler.run(components, SIMULATION_TICK_DURATION);

  EXPECT_EQ(scheduler.takeSystemTimes().front().total,
            FIRST_SYSTEM_TIME_OF_TWO_RUNS);
}

/**
 * Given a measured system that already ran
 * When its time is taken, then taken again
 * Then the second time is zero
 */
TEST(SystemSchedulerTimes, TakingTheTimesStartsCountingAgain) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  SimulatedClock clock;
  SystemScheduler scheduler;
  scheduler.add(
      std::make_unique<ClockAdvancingSystem>(clock, FIRST_SYSTEM_TIME),
      FIRST_SYSTEM_NAME);
  scheduler.measureWith(&clock);
  scheduler.run(components, SIMULATION_TICK_DURATION);

  const std::vector<SystemTime> first = scheduler.takeSystemTimes();
  const std::vector<SystemTime> second = scheduler.takeSystemTimes();

  EXPECT_EQ(first.front().total, FIRST_SYSTEM_TIME);
  EXPECT_EQ(second.front().total, Duration::zero());
}

/**
 * Given a scheduler that was given no clock
 * When it runs a system that takes time
 * Then that time is not counted
 */
TEST(SystemSchedulerTimes, MeasuresNothingWithoutAClock) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  SimulatedClock clock;
  SystemScheduler scheduler;
  scheduler.add(
      std::make_unique<ClockAdvancingSystem>(clock, FIRST_SYSTEM_TIME),
      FIRST_SYSTEM_NAME);

  scheduler.run(components, SIMULATION_TICK_DURATION);

  EXPECT_EQ(scheduler.takeSystemTimes().front().total, Duration::zero());
}

/**
 * Given a scheduler whose clock was taken back
 * When it runs a system that takes time
 * Then that time is not counted
 */
TEST(SystemSchedulerTimes, StopsMeasuringOnceTheClockIsTakenBack) {
  EntityRegistry entities;
  ComponentRegistry components(entities);
  SimulatedClock clock;
  SystemScheduler scheduler;
  scheduler.add(
      std::make_unique<ClockAdvancingSystem>(clock, FIRST_SYSTEM_TIME),
      FIRST_SYSTEM_NAME);
  scheduler.measureWith(&clock);
  scheduler.measureWith(nullptr);

  scheduler.run(components, SIMULATION_TICK_DURATION);

  EXPECT_EQ(scheduler.takeSystemTimes().front().total, Duration::zero());
}

/**
 * Given a system added without a name
 * When the times are taken
 * Then it is listed with an empty name
 */
TEST(SystemSchedulerTimes, ASystemAddedWithoutANameHasAnEmptyName) {
  SimulatedClock clock;
  SystemScheduler scheduler;
  scheduler.add(
      std::make_unique<ClockAdvancingSystem>(clock, FIRST_SYSTEM_TIME));

  EXPECT_TRUE(scheduler.takeSystemTimes().front().name.empty());
}

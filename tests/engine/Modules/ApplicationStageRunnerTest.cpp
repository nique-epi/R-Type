#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ApplicationLoopTestConstants.hpp"
#include "ApplicationStageRunnerFixtures.hpp"
#include "JournalModule.hpp"
#include "LoopStages.hpp"
#include "PacingModule.hpp"
#include "StageTestConstants.hpp"

using rtype::engine::Application;
using rtype::engine::LoopStages;

/**
 * Given a started application whose first module required the IStageRunner
 * during its install(), and whose second module adds its work to simulation
 * When the first module runs one pass of the loop through it
 * Then the work of the second module runs
 */
TEST(ApplicationStageRunner, LetsTheModuleThatPacesTheLoopRunTheStages) {
  JournalModule journal;
  auto pacing = std::make_unique<PacingModule>();
  const PacingModule& pacer = *pacing;
  Application application(
      LoopStages(TEST_LOOP_STAGES),
      pacingThenSecondInSimulation(std::move(pacing), journal));
  application.start();

  pacer.runOnePass(ELAPSED_TIME);

  EXPECT_EQ(journal.calls, std::vector<std::string>{SIMULATION_OF_SECOND});
}

/**
 * Given an application whose first module required the IStageRunner during
 * its install(), started then stopped
 * When the first module runs one pass of the loop through it
 * Then no work runs
 */
TEST(ApplicationStageRunner, RunsNothingOnceTheModulesStopped) {
  JournalModule journal;
  auto pacing = std::make_unique<PacingModule>();
  const PacingModule& pacer = *pacing;
  Application application(
      LoopStages(TEST_LOOP_STAGES),
      pacingThenSecondInSimulation(std::move(pacing), journal));
  application.start();
  application.stop();

  pacer.runOnePass(ELAPSED_TIME);

  EXPECT_TRUE(journal.calls.empty());
}

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "EngineException.hpp"
#include "LoopStages.hpp"
#include "LoopStagesTestConstants.hpp"
#include "StageTestConstants.hpp"
#include "StageWork.hpp"
#include "StageWorkFixtures.hpp"
#include "TimeConstants.hpp"

using rtype::engine::DuplicateStageException;
using rtype::engine::Duration;
using rtype::engine::EmptyStageWorkException;
using rtype::engine::LoopStages;
using rtype::engine::StageWork;
using rtype::engine::UnknownStageException;

/**
 * Given a loop of inputs, simulation and outputs, with a work added to
 * outputs, then inputs, then simulation
 * When it runs
 * Then the work of inputs runs, then simulation, then outputs
 */
TEST(LoopStages, RunsStagesInDeclaredOrder) {
  LoopStages stages(TEST_LOOP_STAGES);
  std::vector<std::string> ran;
  stages.add(OUTPUT_STAGE, recordingWork(ran, OUTPUT_STAGE));
  stages.add(INPUT_STAGE, recordingWork(ran, INPUT_STAGE));
  stages.add(SIMULATION_STAGE, recordingWork(ran, SIMULATION_STAGE));

  stages.run(ELAPSED_TIME);

  EXPECT_EQ(ran, (std::vector<std::string>{INPUT_STAGE, SIMULATION_STAGE,
                                           OUTPUT_STAGE}));
}

/**
 * Given three works added to the simulation stage, first, second, third
 * When the loop runs
 * Then they run first, second, third
 */
TEST(LoopStages, RunsTheWorksOfAStageInTheOrderTheyWereAdded) {
  LoopStages stages(TEST_LOOP_STAGES);
  std::vector<std::string> ran;
  stages.add(SIMULATION_STAGE, recordingWork(ran, FIRST_WORK));
  stages.add(SIMULATION_STAGE, recordingWork(ran, SECOND_WORK));
  stages.add(SIMULATION_STAGE, recordingWork(ran, THIRD_WORK));

  stages.run(ELAPSED_TIME);

  EXPECT_EQ(ran,
            (std::vector<std::string>{FIRST_WORK, SECOND_WORK, THIRD_WORK}));
}

/**
 * Given a work in inputs and a work in outputs
 * When the loop runs with an elapsed time
 * Then each work is given that elapsed time
 */
TEST(LoopStages, GivesEveryWorkTheElapsedTime) {
  LoopStages stages(TEST_LOOP_STAGES);
  std::vector<Duration> elapsedTimes;
  stages.add(INPUT_STAGE, elapsedRecordingWork(elapsedTimes));
  stages.add(OUTPUT_STAGE, elapsedRecordingWork(elapsedTimes));

  stages.run(ELAPSED_TIME);

  EXPECT_EQ(elapsedTimes, (std::vector<Duration>{ELAPSED_TIME, ELAPSED_TIME}));
}

/**
 * Given a loop of inputs, simulation and outputs
 * When a work is added to a stage it does not declare
 * Then an UnknownStageException is thrown
 */
TEST(LoopStages, RefusesWorkForAStageItDoesNotDeclare) {
  LoopStages stages(TEST_LOOP_STAGES);
  std::vector<std::string> ran;

  EXPECT_THROW(stages.add(UNKNOWN_STAGE, recordingWork(ran, FIRST_WORK)),
               UnknownStageException);
}

/**
 * Given a loop of inputs, simulation and outputs
 * When a work is added to a stage it does not declare
 * Then the error names that stage
 */
TEST(LoopStages, UnknownStageErrorNamesTheStage) {
  LoopStages stages(TEST_LOOP_STAGES);
  std::vector<std::string> ran;

  try {
    stages.add(UNKNOWN_STAGE, recordingWork(ran, FIRST_WORK));
    ADD_FAILURE() << "the work was added";
  } catch (const UnknownStageException& error) {
    EXPECT_EQ(std::string{error.what()}, UNKNOWN_STAGE_MESSAGE);
  }
}

/**
 * Given a loop of inputs, simulation and outputs
 * When an empty work is added to inputs
 * Then an EmptyStageWorkException is thrown
 */
TEST(LoopStages, RefusesAnEmptyWork) {
  LoopStages stages(TEST_LOOP_STAGES);

  EXPECT_THROW(stages.add(INPUT_STAGE, StageWork{}), EmptyStageWorkException);
}

/**
 * Given a list of stages that names inputs twice
 * When a loop is built from it
 * Then a DuplicateStageException is thrown
 */
TEST(LoopStages, RefusesAStageDeclaredTwice) {
  EXPECT_THROW(const LoopStages stages(INPUTS_DECLARED_TWICE),
               DuplicateStageException);
}

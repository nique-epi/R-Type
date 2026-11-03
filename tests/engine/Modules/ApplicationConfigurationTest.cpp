#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "Application.hpp"
#include "ApplicationConfigurationFixtures.hpp"
#include "ApplicationConfigurationTestConstants.hpp"
#include "ApplicationFixtures.hpp"
#include "ApplicationLoopFixtures.hpp"
#include "ApplicationLoopTestConstants.hpp"
#include "ApplicationTestConstants.hpp"
#include "CommandInterpreter.hpp"
#include "JournalModule.hpp"
#include "LoopStages.hpp"
#include "StageTestConstants.hpp"

using rtype::engine::Application;
using rtype::engine::CommandInterpreter;
using rtype::engine::LoopStages;
using Lines = std::vector<std::string>;

/**
 * Given an application of three modules and a configuration step
 * When it starts
 * Then the step runs after the three installs and before the three starts
 */
TEST(ApplicationConfiguration, StepRunsBetweenInstallsAndStarts) {
  JournalModule journal;
  Lines refusals;
  Application application(withoutStages(), threeModules(journal),
                          runningLines({}, journal, refusals));

  application.start();

  EXPECT_EQ(journal.calls,
            (Lines{INSTALL_FIRST, INSTALL_SECOND, INSTALL_THIRD, CONFIGURE_STEP,
                   START_FIRST, START_SECOND, START_THIRD}));
}

/**
 * Given an application whose step turns the second module off
 * When it starts
 * Then the second module is installed and never started, the others start
 */
TEST(ApplicationConfiguration, ModuleTurnedOffIsInstalledAndNeverStarted) {
  JournalModule journal;
  Lines refusals;
  Application application(withoutStages(), threeModules(journal),
                          runningLines({TURN_SECOND_OFF}, journal, refusals));

  application.start();

  EXPECT_EQ(journal.calls, (Lines{INSTALL_FIRST, INSTALL_SECOND, INSTALL_THIRD,
                                  CONFIGURE_STEP, START_FIRST, START_THIRD}));
}

/**
 * Given a started application whose second module was turned off
 * When it stops
 * Then only the modules that started are stopped, the last started first
 */
TEST(ApplicationConfiguration, StopsOnlyTheModulesThatStarted) {
  JournalModule journal;
  Lines refusals;
  Application application(withoutStages(), threeModules(journal),
                          runningLines({TURN_SECOND_OFF}, journal, refusals));
  application.start();
  journal.calls.clear();

  application.stop();

  EXPECT_EQ(journal.calls, (Lines{STOP_THIRD, STOP_FIRST}));
}

/**
 * Given a module turned off and on again by the step
 * When the application starts
 * Then the module starts
 */
TEST(ApplicationConfiguration, ModuleTurnedBackOnStarts) {
  JournalModule journal;
  Lines refusals;
  Application application(
      withoutStages(), threeModules(journal),
      runningLines({TURN_SECOND_OFF, TURN_SECOND_ON}, journal, refusals));

  application.start();

  EXPECT_EQ(journal.calls.back(), START_THIRD);
  EXPECT_EQ(journal.calls[journal.calls.size() - 2], START_SECOND);
  EXPECT_TRUE(refusals.empty());
}

/**
 * Given a second module that depends on the first
 * When the step turns the first module off
 * Then the line is refused naming the second module, and both start
 */
TEST(ApplicationConfiguration, ModuleAnotherModuleNeedsCannotBeTurnedOff) {
  JournalModule journal;
  Lines refusals;
  Application application(withoutStages(), secondDependingOnFirst(journal),
                          runningLines({TURN_FIRST_OFF}, journal, refusals));

  application.start();

  ASSERT_EQ(refusals.size(), 1U);
  EXPECT_NE(refusals.front().find(SECOND_NEEDS_FIRST), std::string::npos);
  EXPECT_EQ(journal.calls.back(), START_SECOND);
  EXPECT_EQ(journal.calls[journal.calls.size() - 2], START_FIRST);
}

/**
 * Given a second module that depends on the first, both turned off
 * When the step turns the second back on
 * Then the line is refused, since the first is off, and neither starts
 */
TEST(ApplicationConfiguration, ModuleCannotBeTurnedOnWithItsDependencyOff) {
  JournalModule journal;
  Lines refusals;
  Application application(
      withoutStages(), secondDependingOnFirst(journal),
      runningLines({TURN_SECOND_OFF, TURN_FIRST_OFF, TURN_SECOND_ON}, journal,
                   refusals));

  application.start();

  ASSERT_EQ(refusals.size(), 1U);
  EXPECT_NE(refusals.front().find(FIRST_IS_OFF), std::string::npos);
  EXPECT_EQ(journal.calls.back(), CONFIGURE_STEP);
}

/**
 * Given three modules
 * When the step turns off a module nobody listed
 * Then the line is refused listing the modules, and all three start
 */
TEST(ApplicationConfiguration, UnknownModuleIsRefusedListingTheKnown) {
  JournalModule journal;
  Lines refusals;
  Application application(withoutStages(), threeModules(journal),
                          runningLines({TURN_UNKNOWN_OFF}, journal, refusals));

  application.start();

  ASSERT_EQ(refusals.size(), 1U);
  EXPECT_NE(refusals.front().find(KNOWN_MODULES), std::string::npos);
  EXPECT_EQ(journal.calls.back(), START_THIRD);
}

/**
 * Given three modules
 * When the step gives a word that is neither on nor off
 * Then the line is refused and all three start
 */
TEST(ApplicationConfiguration, WordThatIsNeitherOnNorOffIsRefused) {
  JournalModule journal;
  Lines refusals;
  Application application(withoutStages(), threeModules(journal),
                          runningLines({TURN_SECOND_MAYBE}, journal, refusals));

  application.start();

  ASSERT_EQ(refusals.size(), 1U);
  EXPECT_NE(refusals.front().find(EXPECTED_ON_OR_OFF), std::string::npos);
  EXPECT_EQ(journal.calls.back(), START_THIRD);
}

/**
 * Given an application
 * When its configuration step receives the interpreter
 * Then the interpreter holds the `module` and `set` commands
 */
TEST(ApplicationConfiguration, InterpreterHoldsModuleAndSetCommands) {
  JournalModule journal;
  Lines help;
  Application application(
      withoutStages(), threeModules(journal),
      [&help](CommandInterpreter& interpreter) { help = interpreter.help(); });

  application.start();

  ASSERT_EQ(help.size(), 2U);
  EXPECT_EQ(help[0].rfind(MODULE_HELP_START, 0), 0U);
  EXPECT_EQ(help[1].rfind(SET_HELP_START, 0), 0U);
}

/**
 * Given three modules that each add a simulation work and a step that turns
 * the second off
 * When the started application runs its stages
 * Then the works of the first and the third run and the work of the second,
 * which was added in its install, does not
 */
TEST(ApplicationConfiguration, ModuleTurnedOffRunsNoWork) {
  JournalModule journal;
  Lines refusals;
  Application application(LoopStages(TEST_LOOP_STAGES),
                          threeModulesInSimulation(journal),
                          runningLines({TURN_SECOND_OFF}, journal, refusals));
  application.start();
  journal.calls.clear();

  application.runStages(ELAPSED_TIME);

  EXPECT_EQ(journal.calls, (Lines{SIMULATION_OF_FIRST, SIMULATION_OF_THIRD}));
}

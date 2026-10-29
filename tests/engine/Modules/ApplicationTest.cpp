#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ApplicationFixtures.hpp"
#include "ApplicationTestConstants.hpp"
#include "EngineException.hpp"
#include "IModule.hpp"
#include "JournalModule.hpp"

using rtype::engine::Application;
using rtype::engine::IModule;
using rtype::engine::MissingDependencyException;
using rtype::engine::ModuleStartFailedException;
using rtype::engine::NullModuleException;

/**
 * Given an application of three modules listed first, second, third
 * When it starts
 * Then it installs the three in list order, then starts the three in list order
 */
TEST(Application, InstallsEveryModuleThenStartsEveryModuleInListOrder) {
  JournalModule journal;
  Application application(threeModules(journal));

  application.start();

  EXPECT_EQ(journal.calls, (std::vector<std::string>{
                               INSTALL_FIRST, INSTALL_SECOND, INSTALL_THIRD,
                               START_FIRST, START_SECOND, START_THIRD}));
}

/**
 * Given a started application of three modules listed first, second, third
 * When it stops
 * Then it stops third, then second, then first
 */
TEST(Application, StopsModulesInReverseListOrder) {
  JournalModule journal;
  Application application(threeModules(journal));
  application.start();
  journal.calls.clear();

  application.stop();

  EXPECT_EQ(journal.calls,
            (std::vector<std::string>{STOP_THIRD, STOP_SECOND, STOP_FIRST}));
}

/**
 * Given an application that started and stopped
 * When it stops again
 * Then no module is stopped a second time
 */
TEST(Application, SecondStopStopsNothing) {
  JournalModule journal;
  Application application(threeModules(journal));
  application.start();
  application.stop();
  journal.calls.clear();

  application.stop();

  EXPECT_TRUE(journal.calls.empty());
}

/**
 * Given a started application of three modules listed first, second, third
 * When it is destroyed without being stopped
 * Then it stops third, then second, then first
 */
TEST(Application, DestructionStopsStartedModulesInReverseListOrder) {
  JournalModule journal;
  {
    Application application(threeModules(journal));
    application.start();
    journal.calls.clear();
  }

  EXPECT_EQ(journal.calls,
            (std::vector<std::string>{STOP_THIRD, STOP_SECOND, STOP_FIRST}));
}

/**
 * Given an application that already started
 * When it starts again
 * Then no module is installed or started a second time
 */
TEST(Application, SecondStartDoesNothing) {
  JournalModule journal;
  Application application(threeModules(journal));
  application.start();
  journal.calls.clear();

  application.start();

  EXPECT_TRUE(journal.calls.empty());
}

/**
 * Given an application of no module
 * When it starts, then stops
 * Then nothing throws
 */
TEST(Application, ApplicationWithoutModulesStartsAndStops) {
  JournalModule journal;
  Application application(recordingModules({}, journal));

  EXPECT_NO_THROW(application.start());
  EXPECT_NO_THROW(application.stop());
}

/**
 * Given a list where second depends on a module no entry is named after
 * When an application is built from it
 * Then a MissingDependencyException is thrown
 */
TEST(Application, RefusesAModuleWhoseDependencyIsMissing) {
  JournalModule journal;

  EXPECT_THROW(
      const Application application(secondDependingOnUnlisted(journal)),
      MissingDependencyException);
}

/**
 * Given a list where second depends on a module no entry is named after
 * When an application is built from it
 * Then the error names second and the missing module
 */
TEST(Application, MissingDependencyErrorNamesTheMissingModule) {
  JournalModule journal;

  try {
    const Application application(secondDependingOnUnlisted(journal));
    ADD_FAILURE() << "the application was built";
  } catch (const MissingDependencyException& error) {
    EXPECT_EQ(std::string{error.what()}, SECOND_DEPENDS_ON_UNLISTED_MESSAGE);
  }
}

/**
 * Given a list where second depends on a module no entry is named after
 * When an application is built from it
 * Then no module is installed, started or stopped
 */
TEST(Application, MissingDependencyCallsNoModule) {
  JournalModule journal;

  EXPECT_THROW(
      const Application application(secondDependingOnUnlisted(journal)),
      MissingDependencyException);

  EXPECT_TRUE(journal.calls.empty());
}

/**
 * Given a list where first depends on second, listed after it
 * When an application is built from it
 * Then a MissingDependencyException is thrown
 */
TEST(Application, RefusesADependencyListedAfterTheModuleThatNeedsIt) {
  JournalModule journal;

  EXPECT_THROW(const Application application(firstDependingOnSecond(journal)),
               MissingDependencyException);
}

/**
 * Given a list where second depends on first, listed before it
 * When an application is built from it
 * Then nothing throws
 */
TEST(Application, AcceptsADependencyListedBeforeTheModuleThatNeedsIt) {
  JournalModule journal;

  EXPECT_NO_THROW(
      const Application application(secondDependingOnFirst(journal)));
}

/**
 * Given a list holding a null module
 * When an application is built from it
 * Then a NullModuleException is thrown
 */
TEST(Application, RefusesANullModule) {
  std::vector<std::unique_ptr<IModule>> modules;
  modules.push_back(nullptr);

  EXPECT_THROW(const Application application(std::move(modules)),
               NullModuleException);
}

/**
 * Given an application of first, second and third, where third fails to start
 * When it starts
 * Then second, then first are stopped, and third is not stopped
 */
TEST(Application, FailedStartStopsTheStartedModulesInReverseListOrder) {
  JournalModule journal;
  Application application(thirdFailingToStart(journal));

  EXPECT_THROW(application.start(), ModuleStartFailedException);

  EXPECT_EQ(journal.calls,
            (std::vector<std::string>{INSTALL_FIRST, INSTALL_SECOND,
                                      INSTALL_THIRD, START_FIRST, START_SECOND,
                                      START_THIRD, STOP_SECOND, STOP_FIRST}));
}

/**
 * Given an application of first, second and third, where first fails to start
 * When it starts
 * Then no module is stopped and no later module is started
 */
TEST(Application, FailedStartOfTheFirstModuleStopsNothing) {
  JournalModule journal;
  Application application(firstFailingToStart(journal));

  EXPECT_THROW(application.start(), ModuleStartFailedException);

  EXPECT_EQ(journal.calls,
            (std::vector<std::string>{INSTALL_FIRST, INSTALL_SECOND,
                                      INSTALL_THIRD, START_FIRST}));
}

/**
 * Given an application where third fails to start
 * When it starts
 * Then the error names third and gives the reason third threw
 */
TEST(Application, FailedStartErrorNamesTheFailingModule) {
  JournalModule journal;
  Application application(thirdFailingToStart(journal));

  try {
    application.start();
    ADD_FAILURE() << "the application started";
  } catch (const ModuleStartFailedException& error) {
    EXPECT_EQ(std::string{error.what()}, THIRD_FAILED_TO_START_MESSAGE);
  }
}

/**
 * Given an application whose start failed at third
 * When it stops
 * Then no module is stopped a second time
 */
TEST(Application, StopAfterAFailedStartStopsNothing) {
  JournalModule journal;
  Application application(thirdFailingToStart(journal));
  EXPECT_THROW(application.start(), ModuleStartFailedException);
  journal.calls.clear();

  application.stop();

  EXPECT_TRUE(journal.calls.empty());
}

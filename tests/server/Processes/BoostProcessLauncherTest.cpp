#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include "BoostProcessLauncher.hpp"
#include "ChildProgramConstants.hpp"
#include "IChildProcess.hpp"
#include "ProcessCommand.hpp"
#include "ProcessExit.hpp"
#include "ProcessFixtures.hpp"
#include "ProcessTestConstants.hpp"
#include "RecordingStandardError.hpp"
#include "ServerException.hpp"

using rtype::server::BoostProcessLauncher;
using rtype::server::IChildProcess;
using rtype::server::ProcessCommand;
using rtype::server::ProcessExit;
using rtype::server::ProcessLaunchException;
using ::testing::UnorderedElementsAre;

/**
 * Given a program that ends at once with the exit code 7
 * When the launcher starts it
 * Then the child reports that exit code once it ended
 */
TEST(BoostProcessLauncher, ReportsTheExitCodeOfAProgramThatEnded) {
  BoostProcessLauncher launcher;

  const std::unique_ptr<IChildProcess> child = launcher.launch(
      ProcessCommand{.program = RTYPE_TEST_CHILD,
                     .arguments = {std::string{EXIT_COMMAND},
                                   std::to_string(EXIT_CODE_OF_THE_CHILD)}});

  const std::optional<ProcessExit> exit = waitForExit(*child, PROCESS_WAIT);
  ASSERT_TRUE(exit.has_value());
  EXPECT_EQ(exit.value_or(ProcessExit{}).code, EXIT_CODE_OF_THE_CHILD);
}

/**
 * Given a program that sleeps
 * When the launcher starts it
 * Then the child reports no exit while it runs, and the system lists it
 */
TEST(BoostProcessLauncher, ReportsNothingWhileTheProgramRuns) {
  BoostProcessLauncher launcher;

  const std::unique_ptr<IChildProcess> child = launcher.launch(ProcessCommand{
      .program = RTYPE_TEST_CHILD, .arguments = {std::string{SLEEP_COMMAND}}});

  EXPECT_FALSE(child->exited().has_value());
  EXPECT_TRUE(processIsRunning(child->processId()));
}

/**
 * Given a program that sleeps
 * When the child is terminated
 * Then it reports a non-zero exit and the system no longer lists the process
 */
TEST(BoostProcessLauncher, TerminateEndsAChildThatRuns) {
  BoostProcessLauncher launcher;
  const std::unique_ptr<IChildProcess> child = launcher.launch(ProcessCommand{
      .program = RTYPE_TEST_CHILD, .arguments = {std::string{SLEEP_COMMAND}}});

  child->terminate();

  const std::optional<ProcessExit> exit = waitForExit(*child, PROCESS_WAIT);
  ASSERT_TRUE(exit.has_value());
  EXPECT_NE(exit.value_or(ProcessExit{}).code, 0);
  EXPECT_TRUE(waitForProcessToEnd(child->processId(), PROCESS_WAIT));
}

/**
 * Given a program that sleeps, started by a launcher
 * When the child is destroyed
 * Then the process is no longer running
 */
TEST(BoostProcessLauncher, DestroyingAChildKillsTheProcessThatRuns) {
  BoostProcessLauncher launcher;
  std::unique_ptr<IChildProcess> child = launcher.launch(ProcessCommand{
      .program = RTYPE_TEST_CHILD, .arguments = {std::string{SLEEP_COMMAND}}});
  const std::int64_t processId = child->processId();

  child.reset();

  EXPECT_TRUE(waitForProcessToEnd(processId, PROCESS_WAIT));
}

/**
 * Given a program that writes a line on its standard output and another on
 * its standard error, then ends
 * When the launcher starts it, and the child is destroyed once it ended
 * Then the standard error of this process received both lines
 */
TEST(BoostProcessLauncher, RelaysBothOutputsOfTheChild) {
  const RecordingStandardError standardError;
  BoostProcessLauncher launcher;
  std::unique_ptr<IChildProcess> child = launcher.launch(ProcessCommand{
      .program = RTYPE_TEST_CHILD, .arguments = {std::string{PRINT_COMMAND}}});

  ASSERT_TRUE(waitForExit(*child, PROCESS_WAIT).has_value());
  child.reset();

  EXPECT_THAT(standardError.lines(),
              UnorderedElementsAre(std::string{CHILD_OUTPUT_LINE},
                                   std::string{CHILD_ERROR_LINE}));
}

/**
 * Given a program that does not exist
 * When the launcher starts it
 * Then it throws a ProcessLaunchException that names the program
 */
TEST(BoostProcessLauncher, ThrowsWhenTheProgramDoesNotExist) {
  BoostProcessLauncher launcher;

  try {
    static_cast<void>(launcher.launch(
        ProcessCommand{.program = "no/such/program", .arguments = {}}));
    FAIL() << "the launch did not throw";
  } catch (const ProcessLaunchException& error) {
    EXPECT_NE(std::string{error.what()}.find("no/such/program"),
              std::string::npos);
  }
}

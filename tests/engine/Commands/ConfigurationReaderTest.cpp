#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "ConfigurationReader.hpp"
#include "ConfigurationReaderFixture.hpp"
#include "ConfigurationReaderTestConstants.hpp"
#include "ConfigurationRegistryTestConstants.hpp"
#include "ConfigurationReport.hpp"
#include "ConfigurationStep.hpp"

/**
 * Given a file of commands that exists
 * When it is read
 * Then the report says it was found, counts the lines that ran, and the
 * settings hold the values it gives
 */
TEST_F(ConfigurationReaderFixture, FileOfCommandsIsRun) {
  writeFile(TWO_SETTINGS_FILE);
  const auto report = read(file(), {});
  EXPECT_TRUE(report.fileFound);
  EXPECT_EQ(report.executedLines, TWO_SETTINGS_FILE_RUNNABLE_LINES);
  EXPECT_TRUE(report.refusals.empty());
  EXPECT_EQ(port().integer(), FILE_PORT);
  EXPECT_EQ(nickname().text(), ACCEPTED_NICKNAME);
}

/**
 * Given a file that does not exist
 * When it is read
 * Then the report says it was not found, holds no refusal, and the settings
 * keep their defaults
 */
TEST_F(ConfigurationReaderFixture, MissingFileIsNotAnError) {
  const auto report =
      read(folder().path() / std::string(MISSING_CONFIGURATION_FILE), {});
  EXPECT_FALSE(report.fileFound);
  EXPECT_TRUE(report.refusals.empty());
  EXPECT_EQ(port().integer(), DEFAULT_PORT_VALUE);
}

/**
 * Given a file whose second line is refused
 * When it is read
 * Then the refusal names the file and the line, and the lines after it still
 * run
 */
TEST_F(ConfigurationReaderFixture, RefusedLineDoesNotStopTheReading) {
  writeFile(FILE_WITH_A_BAD_LINE);
  const auto report = read(file(), {});
  ASSERT_EQ(report.refusals.size(), 1U);
  EXPECT_NE(report.refusals.front().find(std::string(CONFIGURATION_FILE)),
            std::string::npos);
  EXPECT_NE(report.refusals.front().find(std::string(BAD_LINE_POSITION)),
            std::string::npos);
  EXPECT_EQ(port().integer(), FILE_PORT);
}

/**
 * Given a file that sets the port and a forced line that sets it again
 * When the file is read with the forced line
 * Then the forced line wins
 */
TEST_F(ConfigurationReaderFixture, ForcedLineOverridesTheFile) {
  writeFile(FILE_SETTING_PORT);
  static_cast<void>(read(file(), {std::string(FORCED_PORT_LINE)}));
  EXPECT_EQ(port().integer(), FORCED_PORT);
}

/**
 * Given a missing file and a forced line
 * When they are read
 * Then the forced line still runs
 */
TEST_F(ConfigurationReaderFixture, ForcedLineRunsWithoutAFile) {
  static_cast<void>(
      read(folder().path() / std::string(MISSING_CONFIGURATION_FILE),
           {std::string(FORCED_PORT_LINE)}));
  EXPECT_EQ(port().integer(), FORCED_PORT);
}

/**
 * Given a forced line that is refused
 * When it is read
 * Then the refusal names the launch option and the old value stays
 */
TEST_F(ConfigurationReaderFixture, RefusedForcedLineNamesTheLaunchOption) {
  const auto report =
      read(folder().path() / std::string(MISSING_CONFIGURATION_FILE),
           {std::string(FORCED_BAD_PORT_LINE)});
  ASSERT_EQ(report.refusals.size(), 1U);
  EXPECT_NE(report.refusals.front().find(std::string(FORCED_LINE_ORIGIN_TEXT)),
            std::string::npos);
  EXPECT_EQ(port().integer(), DEFAULT_PORT_VALUE);
}

/**
 * Given a file written with Windows line ends
 * When it is read
 * Then every line runs and none is refused
 */
TEST_F(ConfigurationReaderFixture, WindowsLineEndsAreAccepted) {
  writeFile(FILE_WITH_WINDOWS_LINE_ENDS);
  const auto report = read(file(), {});
  EXPECT_TRUE(report.refusals.empty());
  EXPECT_EQ(port().integer(), FILE_PORT);
}

/**
 * Given the step of a program built on a file and a forced line
 * When it runs
 * Then it fills the report it was given and the settings hold the values
 */
TEST_F(ConfigurationReaderFixture, StepReadsTheFileAndFillsItsReport) {
  writeFile(FILE_SETTING_PORT);
  rtype::engine::ConfigurationReport report;
  const rtype::engine::ConfigurationStep step =
      rtype::engine::configurationStep(file(), {std::string(FORCED_PORT_LINE)},
                                       report);

  step(interpreter());

  EXPECT_TRUE(report.fileFound);
  EXPECT_EQ(port().integer(), FORCED_PORT);
}

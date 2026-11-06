#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "ConfigurationReader.hpp"
#include "ConfigurationReaderFixture.hpp"
#include "ConfigurationReaderTestConstants.hpp"
#include "ConfigurationRegistryTestConstants.hpp"
#include "ConfigurationReport.hpp"
#include "ConfigurationSource.hpp"
#include "ConfigurationStep.hpp"
#include "HandOverRuleFixtures.hpp"

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

/**
 * Given a file with two lines of a command its rule hands over, between lines
 * it runs, and a forced line of that command
 * When it is read
 * Then the handed-over lines run nothing, are neither counted nor refused, and
 * are kept as written, the lines of the file first
 */
TEST_F(ConfigurationReaderFixture, HandedOverLinesAreKeptInOrderAndNotRun) {
  writeFile(FILE_WITH_HANDED_OVER_LINES);
  const auto report = read(rtype::engine::ConfigurationSource{
      .file = file(),
      .forcedLines = {std::string(FORCED_HANDED_OVER_LINE)},
      .handOver = handingOver({std::string(HANDED_OVER_COMMAND)})});

  EXPECT_TRUE(report.refusals.empty());
  EXPECT_EQ(report.executedLines, FILE_WITH_HANDED_OVER_LINES_RUNNABLE_LINES);
  EXPECT_EQ(report.handedOverLines,
            (std::vector<std::string>{std::string(FIRST_HANDED_OVER_LINE),
                                      std::string(SECOND_HANDED_OVER_LINE),
                                      std::string(FORCED_HANDED_OVER_LINE)}));
  EXPECT_EQ(port().integer(), FILE_PORT);
}

/**
 * Given a rule that hands over one command and a line of no known command
 * When it is read
 * Then the line is refused, as without a rule, and nothing is handed over
 */
TEST_F(ConfigurationReaderFixture, ALineOfNobodyIsStillRefused) {
  const auto report = read(rtype::engine::ConfigurationSource{
      .file = {},
      .forcedLines = {std::string(UNKNOWN_COMMAND_LINE)},
      .handOver = handingOver({std::string(HANDED_OVER_COMMAND)})});

  EXPECT_EQ(report.refusals.size(), ONE_REFUSAL);
  EXPECT_TRUE(report.handedOverLines.empty());
}

/**
 * Given the same file read with no rule
 * When it is read
 * Then every line is run, the lines of the unknown command are refused, and
 * nothing is handed over
 */
TEST_F(ConfigurationReaderFixture, WithoutARuleNothingIsHandedOver) {
  writeFile(FILE_WITH_HANDED_OVER_LINES);
  const auto report = read(file(), {});

  EXPECT_EQ(report.refusals.size(), TWO_REFUSALS);
  EXPECT_TRUE(report.handedOverLines.empty());
}

/**
 * Given an empty file path and no forced line
 * When it is read
 * Then no file is read and the report says none was found
 */
TEST_F(ConfigurationReaderFixture, AnEmptyPathReadsNoFile) {
  const auto report = read(rtype::engine::ConfigurationSource{
      .file = {}, .forcedLines = {}, .handOver = {}});

  EXPECT_FALSE(report.fileFound);
  EXPECT_EQ(report.executedLines, 0U);
}

/**
 * Given the file with lines its rule hands over, and a forced line of that
 * command
 * When only its handed-over lines are read
 * Then they come as written, the lines of the file first, and no line runs
 */
TEST_F(ConfigurationReaderFixture, HandedOverLinesAreReadWithoutRunningAny) {
  writeFile(FILE_WITH_HANDED_OVER_LINES);

  const std::vector<std::string> lines =
      rtype::engine::handedOverLines(rtype::engine::ConfigurationSource{
          .file = file(),
          .forcedLines = {std::string(FORCED_HANDED_OVER_LINE)},
          .handOver = handingOver({std::string(HANDED_OVER_COMMAND)})});

  EXPECT_EQ(lines,
            (std::vector<std::string>{std::string(FIRST_HANDED_OVER_LINE),
                                      std::string(SECOND_HANDED_OVER_LINE),
                                      std::string(FORCED_HANDED_OVER_LINE)}));
  EXPECT_EQ(port().integer(), DEFAULT_PORT_VALUE);
}

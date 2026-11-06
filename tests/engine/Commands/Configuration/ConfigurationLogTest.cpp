#include <gtest/gtest.h>
#include <string>
#include "ConfigurationLog.hpp"
#include "ConfigurationLogFixture.hpp"
#include "ConfigurationLogTestConstants.hpp"
#include "ConfigurationReport.hpp"

using rtype::engine::ConfigurationReport;
using rtype::engine::logConfigurationReport;

/**
 * Given a report of a file that was not found
 * When it is logged
 * Then the journal says the file was not found and the defaults apply
 */
TEST_F(ConfigurationLogFixture, MissingFileIsSaidAndTheDefaultsApply) {
  logConfigurationReport(ConfigurationReport{.fileFound = false,
                                             .executedLines = 0,
                                             .refusals = {},
                                             .handedOverLines = {}},
                         LOGGED_FILE_NAME, logger());

  ASSERT_EQ(lines().size(), 1U);
  EXPECT_NE(lines().front().find(std::string(NOT_FOUND_TEXT)),
            std::string::npos);
}

/**
 * Given a report of a file that was found and ran two commands
 * When it is logged
 * Then the journal names the file and the number of commands
 */
TEST_F(ConfigurationLogFixture, FoundFileIsLoggedWithItsCommandCount) {
  logConfigurationReport(
      ConfigurationReport{.fileFound = true,
                          .executedLines = LOGGED_EXECUTED_LINES,
                          .refusals = {},
                          .handedOverLines = {}},
      LOGGED_FILE_NAME, logger());

  ASSERT_EQ(lines().size(), 1U);
  EXPECT_NE(lines().front().find(std::string(EXECUTED_TEXT)),
            std::string::npos);
  EXPECT_NE(lines().front().find(std::string(LOGGED_FILE_NAME)),
            std::string::npos);
}

/**
 * Given a report holding a refusal
 * When it is logged
 * Then the journal holds the refusal as a warning
 */
TEST_F(ConfigurationLogFixture, RefusedLineIsLoggedAsAWarning) {
  logConfigurationReport(
      ConfigurationReport{.fileFound = true,
                          .executedLines = 0,
                          .refusals = {std::string(LOGGED_REFUSAL)},
                          .handedOverLines = {}},
      LOGGED_FILE_NAME, logger());

  ASSERT_EQ(lines().size(), 2U);
  EXPECT_NE(lines().back().find(std::string(LOGGED_REFUSAL)),
            std::string::npos);
  EXPECT_NE(lines().back().find(std::string(WARNING_LEVEL_TEXT)),
            std::string::npos);
}

/**
 * Given the report of a program that read no file, holding a refusal
 * When it is logged with no file name
 * Then the journal holds the refusal only, and says nothing of a file
 */
TEST_F(ConfigurationLogFixture, NoFileNameTellsOnlyTheRefusals) {
  logConfigurationReport(
      ConfigurationReport{.fileFound = false,
                          .executedLines = 0,
                          .refusals = {std::string(LOGGED_REFUSAL)},
                          .handedOverLines = {}},
      NO_FILE_NAME, logger());

  ASSERT_EQ(lines().size(), 1U);
  EXPECT_NE(lines().front().find(std::string(LOGGED_REFUSAL)),
            std::string::npos);
}

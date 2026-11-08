#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include "Logger.hpp"
#include "LoggerStandardErrorFixture.hpp"
#include "LoggerStandardErrorTestConstants.hpp"
#include "RecordingStandardError.hpp"

using rtype::logging::Logger;
using rtype::logging::LogOutput;
using rtype::logging::writeStandardErrorLine;

/**
 * Given a writer of standard error, and a journal that writes to standard
 * error alone
 * When a logger writes an error
 * Then the writer receives that one line, which ends with the name of the
 * logger and the text
 */
TEST_F(LoggerStandardErrorFixture, AJournalLineReachesTheWriter) {
  const RecordingStandardError recorded;

  Logger{std::string{STANDARD_ERROR_LOGGER_NAME}}.error(DISK_FULL_TEXT);

  const std::vector<std::string> lines = recorded.lines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find(DISK_FULL_LINE_END), std::string::npos);
}

/**
 * Given a writer of standard error
 * When a line that comes from no journal is written to standard error
 * Then the writer receives that line as it is
 */
TEST_F(LoggerStandardErrorFixture, AnotherLineReachesTheWriterAsItIs) {
  const RecordingStandardError recorded;

  writeStandardErrorLine(GAME_LINE);

  EXPECT_EQ(recorded.lines(), std::vector<std::string>{std::string{GAME_LINE}});
}

/**
 * Given a writer of standard error that was set, then taken away
 * When a logger writes an error
 * Then the line reaches std::cerr again
 */
TEST_F(LoggerStandardErrorFixture,
       LinesGoBackToStandardErrorOnceTheWriterLeft) {
  {
    const RecordingStandardError recorded;
  }

  Logger{std::string{STANDARD_ERROR_LOGGER_NAME}}.error(DISK_FULL_TEXT);

  EXPECT_NE(standardError().find(DISK_FULL_LINE_END), std::string::npos);
}

/**
 * Given a writer of standard error
 * When a logger writes an error
 * Then std::cerr receives nothing
 */
TEST_F(LoggerStandardErrorFixture, AWriterTakesTheLinesFromStandardError) {
  const RecordingStandardError recorded;

  Logger{std::string{STANDARD_ERROR_LOGGER_NAME}}.error(DISK_FULL_TEXT);

  EXPECT_TRUE(standardError().empty());
}

/**
 * Given a journal that forces colour on standard error, and a writer of it
 * When a logger writes an error
 * Then the line starts in the colour of an error and ends back in the default
 * colour
 */
TEST_F(LoggerStandardErrorFixture, ForcedColourWrapsTheLine) {
  Logger::setOutput(LogOutput{
      .toStderr = true, .filePath = std::nullopt, .forceColor = true});
  const RecordingStandardError recorded;

  Logger{std::string{STANDARD_ERROR_LOGGER_NAME}}.error(DISK_FULL_TEXT);

  const std::vector<std::string> lines = recorded.lines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_TRUE(lines.front().starts_with(ERROR_COLOR_START));
  EXPECT_TRUE(lines.front().ends_with(DEFAULT_COLOR));
}

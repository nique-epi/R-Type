#include <gtest/gtest.h>
#include <vector>
#include "LogLevel.hpp"
#include "Logger.hpp"
#include "LoggerJournalTestConstants.hpp"
#include "RecordedLine.hpp"
#include "RecordingJournal.hpp"

using rtype::logging::Logger;
using rtype::logging::LogLevel;

/**
 * Given a logger built on a journal that accepts every level
 * When it writes an info line made of a text and a number
 * Then the journal received that one line, with its level, the name of the
 * logger and the text
 */
TEST(LoggerJournal, ALoggerGivenAJournalWritesThroughIt) {
  const RecordingJournal journal;
  const Logger logger{JOURNAL_LOGGER_NAME, journal};

  logger.info(WRITTEN_TEXT, WRITTEN_NUMBER);

  EXPECT_EQ(
      journal.lines(),
      (std::vector<RecordedLine>{RecordedLine{.level = LogLevel::Info,
                                              .loggerName = JOURNAL_LOGGER_NAME,
                                              .body = EXPECTED_BODY}}));
}

/**
 * Given a logger built on a journal that accepts the errors only
 * When it writes an info line
 * Then the journal receives nothing, whatever the level of the program
 */
TEST(LoggerJournal, ALoggerGivenAJournalSkipsWhatTheJournalRefuses) {
  const RecordingJournal journal{LogLevel::Error};
  const Logger logger{JOURNAL_LOGGER_NAME, journal};

  logger.info(REFUSED_TEXT);

  EXPECT_TRUE(journal.lines().empty());
}

#include <gtest/gtest.h>
#include <array>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <vector>
#include "EventField.hpp"
#include "LogEvent.hpp"
#include "LogLevel.hpp"
#include "Logger.hpp"
#include "LoggerJournalTestConstants.hpp"
#include "RecordedLine.hpp"
#include "RecordingJournal.hpp"

using rtype::logging::EventField;
using rtype::logging::LogEvent;
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
                                              .body = EXPECTED_BODY,
                                              .fields = {}}}));
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

/**
 * Given a logger built on a journal that accepts every level
 * When it writes an event with a number, a text and a truth
 * Then the journal receives the message as the body, and the fields as one
 * JSON object: the name of the event, then each value with its type
 */
TEST(LoggerJournal, AnEventReachesTheJournalWithItsFields) {
  const RecordingJournal journal;
  const Logger logger{JOURNAL_LOGGER_NAME, journal};
  const std::array fields{
      EventField{.name = "player", .value = JOINED_PLAYER},
      EventField{.name = "address", .value = std::string_view{JOINED_ADDRESS}},
      EventField{.name = "ready", .value = true}};

  logger.event(LogEvent{.level = LogLevel::Info,
                        .name = JOINED_EVENT,
                        .message = JOINED_MESSAGE,
                        .fields = fields});

  ASSERT_EQ(journal.lines().size(), 1U);
  EXPECT_EQ(journal.lines().front().body, JOINED_MESSAGE);
  const nlohmann::json written =
      nlohmann::json::parse(journal.lines().front().fields);
  EXPECT_EQ(written.size(), 4U);
  EXPECT_EQ(written.at("event").get<std::string>(), JOINED_EVENT);
  EXPECT_EQ(written.at("player").get<std::int64_t>(), JOINED_PLAYER);
  EXPECT_EQ(written.at("address").get<std::string>(), JOINED_ADDRESS);
  EXPECT_TRUE(written.at("ready").get<bool>());
}

/**
 * Given a logger built on a journal that accepts the errors only
 * When it writes an info event
 * Then the journal receives nothing
 */
TEST(LoggerJournal, AnEventBelowTheLevelOfTheJournalIsSkipped) {
  const RecordingJournal journal{LogLevel::Error};
  const Logger logger{JOURNAL_LOGGER_NAME, journal};

  logger.event(LogEvent{.level = LogLevel::Info,
                        .name = JOINED_EVENT,
                        .message = JOINED_MESSAGE,
                        .fields = {}});

  EXPECT_TRUE(journal.lines().empty());
}

#include <gtest/gtest.h>
#include <array>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <vector>
#include "EventField.hpp"
#include "JournalFile.hpp"
#include "LogEvent.hpp"
#include "LogLaunchOptions.hpp"
#include "LogLaunchOptionsFixture.hpp"
#include "LogLaunchOptionsFixtures.hpp"
#include "LogLevel.hpp"
#include "Logger.hpp"
#include "LoggerEventFileTestConstants.hpp"
#include "LoggingConstants.hpp"

using rtype::logging::EventField;
using rtype::logging::FORMAT_OPTION;
using rtype::logging::LogEvent;
using rtype::logging::Logger;
using rtype::logging::LogLevel;

/**
 * Given a journal written as JSON Lines
 * When a warning event is written whose message starts like a size and holds
 * what looks like fields
 * Then the line of the file holds that message as it is, the name of the
 * event and its field, and standard error shows the message alone, as text
 */
TEST_F(LogLaunchOptionsFixture, AnEventKeepsItsMessageAndItsFieldsApart) {
  const JournalFile journal;
  logOptionsOf(std::vector<std::string_view>{FORMAT_OPTION, JSON_FORMAT_VALUE},
               journal.path())
      .apply();
  const std::array fields{
      EventField{.name = "player", .value = TIMED_OUT_PLAYER}};

  Logger{std::string{EVENT_LOGGER_NAME}}.event(
      LogEvent{.level = LogLevel::Warn,
               .name = TIMED_OUT_EVENT,
               .message = TRICKY_MESSAGE,
               .fields = fields});

  const std::vector<std::string> lines = journal.lines();
  ASSERT_EQ(lines.size(), 1U);
  const nlohmann::json line = nlohmann::json::parse(lines.front());
  EXPECT_EQ(line.at("message").get<std::string>(), TRICKY_MESSAGE);
  EXPECT_EQ(line.at("event").get<std::string>(), TIMED_OUT_EVENT);
  EXPECT_EQ(line.at("player").get<std::int64_t>(), TIMED_OUT_PLAYER);
  EXPECT_NE(standardError().find(TRICKY_TEXT_LINE_END), std::string::npos);
}

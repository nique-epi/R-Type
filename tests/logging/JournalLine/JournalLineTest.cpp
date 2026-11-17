#include <gtest/gtest.h>
#include <array>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include "EventField.hpp"
#include "JournalLine.hpp"
#include "JournalLineFixtures.hpp"
#include "JournalLineParts.hpp"
#include "JournalLineTestConstants.hpp"
#include "LogEvent.hpp"
#include "LogLevel.hpp"

using rtype::logging::EventField;
using rtype::logging::eventFieldsText;
using rtype::logging::JournalLineParts;
using rtype::logging::jsonLineOf;
using rtype::logging::LogEvent;
using rtype::logging::LogLevel;

/**
 * Given the parts of a line
 * When it is written as JSON
 * Then it is one object holding time, level, logger and message, in that
 * order, with their values
 */
TEST(JournalLineTest, WritesTheFourMembersInOrder) {
  const std::string line = jsonLineOf(partsWithMessage(LINE_MESSAGE));

  EXPECT_EQ(line, PLAIN_LINE);
}

/**
 * Given a message with a quote, a backslash and an end of line
 * When the line is written as JSON
 * Then it stays on one line, and reading it gives the message back
 */
TEST(JournalLineTest, EscapesQuotesBackslashesAndEndsOfLine) {
  const std::string line = jsonLineOf(partsWithMessage(ESCAPED_MESSAGE));

  EXPECT_EQ(line.find('\n'), std::string::npos);
  EXPECT_EQ(nlohmann::json::parse(line).at("message").get<std::string>(),
            ESCAPED_MESSAGE);
}

/**
 * Given a message written to close its value and add a member
 * When the line is written as JSON
 * Then the object still holds four members, and the message is that text
 */
TEST(JournalLineTest, AForgedMessageAddsNoMember) {
  const std::string line = jsonLineOf(partsWithMessage(FORGED_MESSAGE));

  const nlohmann::json object = nlohmann::json::parse(line);
  EXPECT_EQ(object.size(), 4U);
  EXPECT_EQ(object.at("message").get<std::string>(), FORGED_MESSAGE);
}

/**
 * Given a message holding a byte that is not UTF-8
 * When the line is written as JSON
 * Then it is valid JSON whose message has U+FFFD in place of that byte
 */
TEST(JournalLineTest, ReplacesAByteThatIsNotUtf8) {
  const std::string line = jsonLineOf(partsWithMessage(INVALID_UTF8_MESSAGE));

  EXPECT_EQ(nlohmann::json::parse(line).at("message").get<std::string>(),
            REPLACED_UTF8_MESSAGE);
}

/**
 * Given an event with a number, a text and a truth
 * When its fields are written as JSON
 * Then the object names the event first, then each value in order, with its
 * JSON type
 */
TEST(JournalLineTest, WritesTheNameOfAnEventThenItsTypedValues) {
  const std::array fields{EventField{.name = "player", .value = EVENT_PLAYER},
                          EventField{.name = "address", .value = EVENT_ADDRESS},
                          EventField{.name = "away", .value = false}};

  const std::string text = eventFieldsText(LogEvent{.level = LogLevel::Info,
                                                    .name = EVENT_NAME,
                                                    .message = LINE_MESSAGE,
                                                    .fields = fields});

  EXPECT_EQ(text, EVENT_FIELDS_TEXT);
}

/**
 * Given an event whose fields are named like the members every line has
 * When its fields are written as JSON
 * Then those fields are left out, and the name of the event stays its own
 */
TEST(JournalLineTest, LeavesOutAFieldNamedLikeAMemberOfEveryLine) {
  const std::array fields{
      EventField{.name = "time", .value = CLASHING_VALUE},
      EventField{.name = "level", .value = CLASHING_VALUE},
      EventField{.name = "logger", .value = CLASHING_VALUE},
      EventField{.name = "message", .value = CLASHING_VALUE},
      EventField{.name = "event", .value = CLASHING_VALUE}};

  const std::string text = eventFieldsText(LogEvent{.level = LogLevel::Info,
                                                    .name = EVENT_NAME,
                                                    .message = LINE_MESSAGE,
                                                    .fields = fields});

  EXPECT_EQ(nlohmann::json::parse(text),
            (nlohmann::json{{"event", EVENT_NAME}}));
}

/**
 * Given the parts of a line and the fields of its event
 * When the line is written as JSON
 * Then it is one object: the four members of every line, then the members of
 * the event, in order
 */
TEST(JournalLineTest, AddsTheMembersOfTheEventAfterTheMessage) {
  JournalLineParts parts = partsWithMessage(LINE_MESSAGE);
  parts.fields = EVENT_FIELDS_TEXT;

  const std::string line = jsonLineOf(parts);

  EXPECT_EQ(line, EVENT_LINE);
}

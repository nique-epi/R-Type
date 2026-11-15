#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <vector>
#include "JournalLine.hpp"
#include "JournalLineParts.hpp"
#include "JournalLineTestConstants.hpp"

using rtype::logging::JournalLineParts;
using rtype::logging::jsonLineOf;

namespace {

/** @returns The parts of a line whose message is @p message. */
JournalLineParts partsWithMessage(std::string_view message) {
  return JournalLineParts{.time = LINE_TIME,
                          .level = LINE_LEVEL,
                          .logger = LINE_LOGGER,
                          .message = message};
}

/** @returns The names of the members of @p object, in order. */
std::vector<std::string> keysOf(const nlohmann::ordered_json& object) {
  std::vector<std::string> keys;
  for (const auto& [key, value] : object.items()) {
    keys.push_back(key);
  }
  return keys;
}

}  // namespace

/**
 * Given the parts of a line
 * When it is written as JSON
 * Then it is one object holding time, level, logger and message, in that
 * order, with their values
 */
TEST(JournalLineTest, WritesTheFourMembersInOrder) {
  const std::string line = jsonLineOf(partsWithMessage(LINE_MESSAGE));

  const nlohmann::ordered_json object = nlohmann::ordered_json::parse(line);
  EXPECT_EQ(keysOf(object),
            (std::vector<std::string>{"time", "level", "logger", "message"}));
  EXPECT_EQ(object.at("time"), LINE_TIME);
  EXPECT_EQ(object.at("level"), LINE_LEVEL);
  EXPECT_EQ(object.at("logger"), LINE_LOGGER);
  EXPECT_EQ(object.at("message"), LINE_MESSAGE);
}

/**
 * Given a message with a quote, a backslash and an end of line
 * When the line is written as JSON
 * Then it stays on one line, and reading it gives the message back
 */
TEST(JournalLineTest, EscapesQuotesBackslashesAndEndsOfLine) {
  const std::string line = jsonLineOf(partsWithMessage(ESCAPED_MESSAGE));

  EXPECT_EQ(line.find('\n'), std::string::npos);
  EXPECT_EQ(nlohmann::json::parse(line).at("message"), ESCAPED_MESSAGE);
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
  EXPECT_EQ(object.at("message"), FORGED_MESSAGE);
}

/**
 * Given a message holding a byte that is not UTF-8
 * When the line is written as JSON
 * Then it is valid JSON whose message has U+FFFD in place of that byte
 */
TEST(JournalLineTest, ReplacesAByteThatIsNotUtf8) {
  const std::string line = jsonLineOf(partsWithMessage(INVALID_UTF8_MESSAGE));

  EXPECT_EQ(nlohmann::json::parse(line).at("message"), REPLACED_UTF8_MESSAGE);
}

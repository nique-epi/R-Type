#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include "CommandLine.hpp"
#include "EngineException.hpp"
#include "SettingLine.hpp"
#include "SettingLineTestConstants.hpp"

using rtype::engine::CommandRefusedException;
using rtype::engine::settingLine;
using rtype::engine::settingLineFromAssignment;
using rtype::engine::splitCommandLine;

/**
 * Given a name and a plain value
 * When the line is built
 * Then it is `set` followed by the name and the value
 */
TEST(SettingLine, PlainValueIsWrittenAsIs) {
  EXPECT_EQ(settingLine(LINE_SETTING_NAME, LINE_PLAIN_VALUE),
            LINE_FOR_PLAIN_VALUE);
}

/**
 * Given a value holding a space
 * When the line is built
 * Then the value is wrapped in quotes
 */
TEST(SettingLine, ValueWithASpaceIsQuoted) {
  EXPECT_EQ(settingLine(LINE_SETTING_NAME, LINE_SPACED_VALUE),
            LINE_FOR_SPACED_VALUE);
}

/**
 * Given a value holding a comment mark
 * When the line is built
 * Then the value is wrapped in quotes
 */
TEST(SettingLine, ValueWithACommentMarkIsQuoted) {
  EXPECT_EQ(settingLine(LINE_SETTING_NAME, LINE_MARKED_VALUE),
            LINE_FOR_MARKED_VALUE);
}

/**
 * Given an empty value
 * When the line is built
 * Then the value is an empty quoted word
 */
TEST(SettingLine, EmptyValueIsQuoted) {
  EXPECT_EQ(settingLine(LINE_SETTING_NAME, ""), LINE_FOR_EMPTY_VALUE);
}

/**
 * Given a value holding a quote
 * When the line is built
 * Then the value is quoted with the quote escaped
 */
TEST(SettingLine, ValueWithAQuoteIsQuotedAndEscaped) {
  EXPECT_EQ(settingLine(LINE_SETTING_NAME, LINE_QUOTED_VALUE),
            LINE_FOR_QUOTED_VALUE);
}

/**
 * Given a value holding a backslash and a space
 * When the line is built
 * Then the value is quoted with the backslash escaped
 */
TEST(SettingLine, BackslashInAQuotedValueIsEscaped) {
  EXPECT_EQ(settingLine(LINE_SETTING_NAME, LINE_BACKSLASH_VALUE),
            LINE_FOR_BACKSLASH_VALUE);
}

/**
 * Given a value holding a backslash and nothing that needs quotes
 * When the line is built
 * Then the value is written as is
 */
TEST(SettingLine, BackslashInAnUnquotedValueIsKept) {
  EXPECT_EQ(settingLine(LINE_SETTING_NAME, LINE_PLAIN_BACKSLASH_VALUE),
            LINE_FOR_PLAIN_BACKSLASH_VALUE);
}

/**
 * Given values that need quotes, escapes, or none
 * When each line is built and split back into words
 * Then the last word is the value, exactly as given
 */
TEST(SettingLine, SplittingTheLineGivesTheValueBack) {
  for (const std::string_view value :
       {LINE_PLAIN_VALUE, LINE_SPACED_VALUE, LINE_MARKED_VALUE,
        LINE_QUOTED_VALUE, LINE_BACKSLASH_VALUE, LINE_PLAIN_BACKSLASH_VALUE}) {
    EXPECT_EQ(splitCommandLine(settingLine(LINE_SETTING_NAME, value)).back(),
              std::string(value));
  }
}

/**
 * Given an assignment `name=value`
 * When the line is built
 * Then it is the line of that name and value
 */
TEST(SettingLine, AssignmentGivesTheLineOfItsNameAndValue) {
  EXPECT_EQ(settingLineFromAssignment(ASSIGNMENT), LINE_FOR_PLAIN_VALUE);
}

/**
 * Given an assignment whose value holds another `=`
 * When the line is built
 * Then the first `=` ends the name
 */
TEST(SettingLine, FirstMarkEndsTheName) {
  EXPECT_EQ(settingLineFromAssignment(ASSIGNMENT_WITH_MARK_IN_VALUE),
            LINE_FOR_MARK_IN_VALUE);
}

/**
 * Given an assignment with an empty value
 * When the line is built
 * Then the value is an empty quoted word
 */
TEST(SettingLine, AssignmentWithAnEmptyValueGivesAQuotedEmptyWord) {
  EXPECT_EQ(settingLineFromAssignment(ASSIGNMENT_WITH_EMPTY_VALUE),
            LINE_FOR_EMPTY_VALUE);
}

/**
 * Given an assignment without `=`
 * When the line is built
 * Then it is refused
 */
TEST(SettingLine, AssignmentWithoutAMarkIsRefused) {
  EXPECT_THROW(
      static_cast<void>(settingLineFromAssignment(ASSIGNMENT_WITHOUT_MARK)),
      CommandRefusedException);
}

/**
 * Given an assignment without a name
 * When the line is built
 * Then it is refused
 */
TEST(SettingLine, AssignmentWithoutANameIsRefused) {
  EXPECT_THROW(
      static_cast<void>(settingLineFromAssignment(ASSIGNMENT_WITHOUT_NAME)),
      CommandRefusedException);
}

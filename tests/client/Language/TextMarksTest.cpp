#include <gtest/gtest.h>
#include <string>
#include "TextMarks.hpp"
#include "TextMarksTestConstants.hpp"

using rtype::client::withMark;

/**
 * Given a sentence holding a mark in its middle
 * When the mark is given a value
 * Then the value stands where the mark stood
 */
TEST(TextMarksTest, PutsTheValueWhereTheMarkStands) {
  EXPECT_EQ(withMark(std::string(MARK_IN_THE_MIDDLE), THREE_AS_COUNT),
            MIDDLE_FILLED);
}

/**
 * Given a sentence holding the same mark twice
 * When the mark is given a value
 * Then only the first mark is replaced
 */
TEST(TextMarksTest, ReplacesTheFirstMarkOnly) {
  EXPECT_EQ(withMark(std::string(MARK_TWICE), THREE_AS_COUNT),
            FIRST_OF_TWO_FILLED);
}

/**
 * Given a sentence without the mark
 * When the mark is given a value
 * Then the sentence is unchanged
 */
TEST(TextMarksTest, LeavesASentenceWithoutTheMark) {
  EXPECT_EQ(withMark(std::string(NO_MARK), THREE_AS_COUNT), NO_MARK);
}

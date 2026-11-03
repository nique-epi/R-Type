#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "NameList.hpp"
#include "NameListTestConstants.hpp"

using rtype::engine::joinedNames;

/**
 * Given three names
 * When they are joined
 * Then they come on one line, separated by a comma and a space
 */
TEST(NameList, NamesAreSeparatedByACommaAndASpace) {
  const std::vector<std::string> names{std::string(FIRST_NAME),
                                       std::string(SECOND_NAME),
                                       std::string(THIRD_NAME)};

  EXPECT_EQ(joinedNames(names), THREE_NAMES_LINE);
}

/**
 * Given one name
 * When it is joined
 * Then it comes alone, with no separator
 */
TEST(NameList, OneNameHasNoSeparator) {
  const std::vector<std::string> names{std::string(FIRST_NAME)};

  EXPECT_EQ(joinedNames(names), FIRST_NAME);
}

/**
 * Given no name
 * When they are joined
 * Then the line is empty
 */
TEST(NameList, NoNameGivesAnEmptyLine) { EXPECT_TRUE(joinedNames({}).empty()); }

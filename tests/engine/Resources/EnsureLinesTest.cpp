#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "EngineException.hpp"
#include "EnsureLines.hpp"
#include "EnsureLinesTestConstants.hpp"
#include "ResourceFixtures.hpp"

using rtype::engine::ensuredResources;
using rtype::engine::ensureLineRule;
using rtype::engine::InvalidEnsureLineException;
using rtype::engine::InvalidResourceNameException;

/**
 * Given lines that start two resources, name the first twice and hold other
 * commands and a comment
 * When the resources they start are read
 * Then each resource comes once, in the order of its first line
 */
TEST(EnsureLines, ListsEachResourceOnceInTheOrderOfItsFirstLine) {
  const std::vector<std::string> resources =
      ensuredResources(textsOf(LINES_STARTING_TWO_RESOURCES));

  EXPECT_EQ(resources, textsOf(RESOURCES_OF_THOSE_LINES));
}

/**
 * Given an `ensure` line that names no resource
 * When the resources it starts are read
 * Then the read is refused
 */
TEST(EnsureLines, RefusesALineThatNamesNoResource) {
  EXPECT_THROW(
      static_cast<void>(ensuredResources(textsOf(ENSURE_WITHOUT_NAME))),
      InvalidEnsureLineException);
}

/**
 * Given an `ensure` line that names two resources
 * When the resources it starts are read
 * Then the read is refused
 */
TEST(EnsureLines, RefusesALineThatNamesTwoResources) {
  EXPECT_THROW(
      static_cast<void>(ensuredResources(textsOf(ENSURE_WITH_TWO_NAMES))),
      InvalidEnsureLineException);
}

/**
 * Given an `ensure` line whose name holds uppercase letters
 * When the resources it starts are read
 * Then the read is refused for the name
 */
TEST(EnsureLines, RefusesANameThatIsNotAResourceName) {
  EXPECT_THROW(
      static_cast<void>(ensuredResources(textsOf(ENSURE_WITH_UPPERCASE))),
      InvalidResourceNameException);
}

/**
 * Given the rule that keeps the `ensure` lines
 * When it reads an `ensure` line
 * Then it keeps it
 */
TEST(EnsureLines, TheRuleKeepsAnEnsureLine) {
  EXPECT_TRUE(ensureLineRule()(textsOf(ENSURE_WORDS)));
}

/**
 * Given the rule that keeps the `ensure` lines
 * When it reads a `set` line
 * Then it leaves it to the program
 */
TEST(EnsureLines, TheRuleLeavesAnyOtherLine) {
  EXPECT_FALSE(ensureLineRule()(textsOf(SET_WORDS)));
}

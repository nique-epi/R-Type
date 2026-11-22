#include <gtest/gtest.h>
#include "LetterCounts.hpp"
#include "LetterCountsTestConstants.hpp"

using rtype::client::lettersIn;
using rtype::client::longestLettersIn;

/**
 * Given a word whose accented letter takes two bytes
 * When its letters are counted
 * Then the accented letter counts once
 */
TEST(LetterCountsTest, AnAccentedLetterCountsOnce) {
  EXPECT_EQ(lettersIn(FOUR_LETTERS_ONE_ACCENT), FOUR_LETTERS);
}

/**
 * Given a text longer in French than in English
 * When its longest language is counted
 * Then it counts the letters of the French
 */
TEST(LetterCountsTest, TheLongestLanguageIsCounted) {
  EXPECT_EQ(longestLettersIn(LONGER_IN_FRENCH), LETTERS_OF_PREPARE);
}

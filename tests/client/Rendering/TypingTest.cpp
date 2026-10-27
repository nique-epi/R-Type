#include <gtest/gtest.h>
#include "Typing.hpp"
#include "TypingTestConstants.hpp"

using rtype::client::letterCountOf;
using rtype::client::lettersTypedAt;
using rtype::client::typedPrefix;
using rtype::client::testing::ACCENTED_FIRST_TWO;
using rtype::client::testing::ACCENTED_NAME;
using rtype::client::testing::ACCENTED_NAME_LETTERS;
using rtype::client::testing::FIVE_LETTERS;
using rtype::client::testing::LETTER_TIME;
using rtype::client::testing::LONG_AFTER;
using rtype::client::testing::ONE_NANOSECOND_EARLY;
using rtype::client::testing::THREE_LETTERS;
using rtype::client::testing::TWO_LETTER_TIMES;
using rtype::client::testing::TWO_LETTERS;

/**
 * Given a text whose typing has not started
 * When its letters are counted
 * Then none is written
 */
TEST(Typing, NoLetterBeforeTheStart) {
  EXPECT_EQ(lettersTypedAt(ONE_NANOSECOND_EARLY, LETTER_TIME, FIVE_LETTERS),
            0U);
}

/**
 * Given a text whose typing starts now
 * When its letters are counted
 * Then its first letter is written at once
 */
TEST(Typing, TheFirstLetterShowsAtOnce) {
  EXPECT_EQ(lettersTypedAt({}, LETTER_TIME, FIVE_LETTERS), 1U);
}

/**
 * Given a text typed one letter every 50 ms
 * When 100 ms passed
 * Then three letters are written
 */
TEST(Typing, OneMoreLetterEachLetterTime) {
  EXPECT_EQ(lettersTypedAt(TWO_LETTER_TIMES, LETTER_TIME, FIVE_LETTERS),
            THREE_LETTERS);
}

/**
 * Given a text of five letters typed long ago
 * When its letters are counted
 * Then five are written, never more
 */
TEST(Typing, NeverMoreThanTheText) {
  EXPECT_EQ(lettersTypedAt(LONG_AFTER, LETTER_TIME, FIVE_LETTERS),
            FIVE_LETTERS);
}

/**
 * Given a name whose first character takes two bytes
 * When its letters are counted
 * Then that character counts once
 */
TEST(Typing, ACharacterOfSeveralBytesIsOneLetter) {
  EXPECT_EQ(letterCountOf(ACCENTED_NAME), ACCENTED_NAME_LETTERS);
}

/**
 * Given a name whose first character takes two bytes
 * When its first two letters are taken
 * Then both bytes of that character are kept
 */
TEST(Typing, APrefixNeverCutsACharacter) {
  EXPECT_EQ(typedPrefix(ACCENTED_NAME, TWO_LETTERS), ACCENTED_FIRST_TWO);
}

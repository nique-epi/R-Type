#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "CommandLine.hpp"
#include "CommandLineTestConstants.hpp"

using rtype::engine::splitCommandLine;
using Words = std::vector<std::string>;

/**
 * Given a line of three words separated by spaces and a tab
 * When it is split
 * Then the three words come back
 */
TEST(CommandLine, WordsAreSeparatedByWhitespace) {
  EXPECT_EQ(splitCommandLine(THREE_WORDS_LINE), (Words{"set", "port", "6000"}));
}

/**
 * Given a line with a word wrapped in quotes and holding a space
 * When it is split
 * Then the quoted word comes back as one word without its quotes
 */
TEST(CommandLine, QuotesKeepTheSpacesOfAWord) {
  EXPECT_EQ(splitCommandLine(QUOTED_LINE),
            (Words{"set", "nickname", "Captain Stardust"}));
}

/**
 * Given a line with a comment after its words
 * When it is split
 * Then only the words before the comment come back
 */
TEST(CommandLine, CommentEndsTheLine) {
  EXPECT_EQ(splitCommandLine(TRAILING_COMMENT_LINE),
            (Words{"set", "port", "6000"}));
}

/**
 * Given a line whose comment mark is inside quotes
 * When it is split
 * Then the mark stays in the word
 */
TEST(CommandLine, CommentMarkInQuotesIsPartOfTheWord) {
  EXPECT_EQ(splitCommandLine(QUOTED_MARK_LINE),
            (Words{"set", "nickname", "a#b"}));
}

/**
 * Given a line that leaves its quote open
 * When it is split
 * Then the open quote ends with the line
 */
TEST(CommandLine, OpenQuoteEndsWithTheLine) {
  EXPECT_EQ(splitCommandLine(OPEN_QUOTE_LINE),
            (Words{"set", "nickname", "Captain Star"}));
}

/**
 * Given a line holding an empty quoted word
 * When it is split
 * Then the empty word comes back
 */
TEST(CommandLine, EmptyQuotesGiveAnEmptyWord) {
  EXPECT_EQ(splitCommandLine(EMPTY_QUOTED_LINE),
            (Words{"set", "nickname", ""}));
}

/**
 * Given an empty line
 * When it is split
 * Then no word comes back
 */
TEST(CommandLine, EmptyLineHasNoWord) {
  EXPECT_TRUE(splitCommandLine("").empty());
}

/**
 * Given a quoted word holding an escaped quote
 * When it is split
 * Then the quote is part of the word and does not end it
 */
TEST(CommandLine, EscapedQuoteIsPartOfTheWord) {
  EXPECT_EQ(splitCommandLine(ESCAPED_QUOTE_LINE),
            (Words{"set", "nickname", "a\"b"}));
}

/**
 * Given a quoted word holding an escaped backslash before the closing quote
 * When it is split
 * Then the backslash is part of the word and the quote ends it
 */
TEST(CommandLine, EscapedBackslashIsPartOfTheWord) {
  EXPECT_EQ(splitCommandLine(ESCAPED_BACKSLASH_LINE),
            (Words{"set", "nickname", "a\\"}));
}

/**
 * Given a quoted word holding a backslash before an ordinary character
 * When it is split
 * Then the backslash stays in the word
 */
TEST(CommandLine, BackslashBeforeAnOrdinaryCharacterIsPlain) {
  EXPECT_EQ(splitCommandLine(PLAIN_BACKSLASH_IN_QUOTES_LINE),
            (Words{"set", "nickname", "a\\b c"}));
}

/**
 * Given a word outside quotes holding a backslash
 * When it is split
 * Then the backslash stays in the word
 */
TEST(CommandLine, BackslashOutsideQuotesIsPlain) {
  EXPECT_EQ(splitCommandLine(BACKSLASH_OUTSIDE_QUOTES_LINE),
            (Words{"set", "path", "a\\b"}));
}

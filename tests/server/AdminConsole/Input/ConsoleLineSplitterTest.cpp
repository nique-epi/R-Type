#include <gtest/gtest.h>
#include <span>
#include <string>
#include <vector>
#include "AdminConsoleTestConstants.hpp"
#include "ConsoleChunk.hpp"
#include "ConsoleLineSplitter.hpp"
#include "ConsoleLineSplitterTestConstants.hpp"
#include "ServerModuleTestConstants.hpp"

using rtype::server::ConsoleChunk;
using rtype::server::ConsoleLineSplitter;

/**
 * Given bytes holding two lines, the first ended the Windows way
 * When they are fed
 * Then both lines come out, without their ends of line
 */
TEST(ConsoleLineSplitter, CutsLinesAndRemovesTheCarriageReturn) {
  ConsoleLineSplitter splitter;

  const ConsoleChunk chunk =
      splitter.feed(std::span<const char>{GAMES_THEN_PLAYERS_FROM_WINDOWS});

  EXPECT_EQ(chunk.lines, (std::vector<std::string>{std::string{GAMES_LINE},
                                                   std::string{PLAYERS_LINE}}));
}

/**
 * Given a line that arrives in two reads
 * When the first read is fed, then the second
 * Then nothing comes out of the first, and the whole line out of the second
 */
TEST(ConsoleLineSplitter, KeepsAPartOfALineForTheNextRead) {
  ConsoleLineSplitter splitter;

  const ConsoleChunk first =
      splitter.feed(std::span<const char>{FIRST_PART_OF_CLOSE});
  const ConsoleChunk second =
      splitter.feed(std::span<const char>{SECOND_PART_OF_CLOSE});

  EXPECT_TRUE(first.lines.empty());
  EXPECT_EQ(second.lines,
            (std::vector<std::string>{std::string{CLOSE_FIRST_PLACE_LINE}}));
}

/**
 * Given a line of exactly the longest length, then one byte longer, then a
 * short one
 * When they are fed
 * Then the first and the last come out, and one line is counted dropped
 */
TEST(ConsoleLineSplitter, DropsALineLongerThanTheLimitWhole) {
  ConsoleLineSplitter splitter;
  const std::string longest(LONGEST_LINE_LENGTH, FILLER);
  const std::string tooLong(LONGEST_LINE_LENGTH + 1, FILLER);
  const std::string bytes =
      longest + "\n" + tooLong + "\n" + std::string{GAMES_LINE} + "\n";

  const ConsoleChunk chunk = splitter.feed(std::span<const char>{bytes});

  EXPECT_EQ(chunk.lines,
            (std::vector<std::string>{longest, std::string{GAMES_LINE}}));
  EXPECT_EQ(chunk.droppedLines, 1U);
}

/**
 * Given a last line without an end of line
 * When the input ends
 * Then that line comes out
 */
TEST(ConsoleLineSplitter, FinishesTheLastLineWithoutAnEndOfLine) {
  ConsoleLineSplitter splitter;
  static_cast<void>(splitter.feed(std::span<const char>{NETWORK_LINE}));

  const ConsoleChunk chunk = splitter.finish();

  EXPECT_EQ(chunk.lines, (std::vector<std::string>{std::string{NETWORK_LINE}}));
}

/**
 * Given bytes that all ended their lines
 * When the input ends
 * Then no line comes out
 */
TEST(ConsoleLineSplitter, FinishesWithNothingWhenEveryLineEnded) {
  ConsoleLineSplitter splitter;
  static_cast<void>(
      splitter.feed(std::span<const char>{GAMES_THEN_PLAYERS_FROM_WINDOWS}));

  EXPECT_TRUE(splitter.finish().lines.empty());
}

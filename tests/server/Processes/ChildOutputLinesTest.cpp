#include <gtest/gtest.h>
#include <optional>
#include <span>
#include <string>
#include <vector>
#include "ChildOutputLines.hpp"
#include "ChildOutputLinesTestConstants.hpp"

using rtype::server::ChildOutputLines;

/**
 * Given a line read in two parts, the end of line in the second
 * When both parts are fed
 * Then the first gives no line and the second gives the whole line
 */
TEST(ChildOutputLines, JoinsALineReadInTwoParts) {
  ChildOutputLines lines;

  const std::vector<std::string> first = lines.feed(FIRST_PART);
  const std::vector<std::string> second = lines.feed(SECOND_PART);

  EXPECT_TRUE(first.empty());
  EXPECT_EQ(second, std::vector<std::string>{std::string{JOINED_LINE}});
}

/**
 * Given a line ended by "\r\n"
 * When it is fed
 * Then the line comes without the '\r'
 */
TEST(ChildOutputLines, DropsTheCarriageReturnBeforeTheEnd) {
  ChildOutputLines lines;

  EXPECT_EQ(lines.feed(WINDOWS_LINE),
            std::vector<std::string>{std::string{READY_LINE}});
}

/**
 * Given a text one byte shorter than the longest line, with no end of line
 * When it is fed
 * Then no line comes yet
 */
TEST(ChildOutputLines, KeepsALineOneByteShortOfTheLimit) {
  ChildOutputLines lines;
  const std::string text(LONGEST_RELAYED_LINE - 1, FILLING_BYTE);

  EXPECT_TRUE(lines.feed(text).empty());
}

/**
 * Given a text three bytes longer than the longest line, with no end of line
 * When it is fed, then the output closes
 * Then a line of the longest length comes, then the three bytes left
 */
TEST(ChildOutputLines, CutsALineAtTheLimit) {
  ChildOutputLines lines;
  const std::string text(OVERLONG_LINE, FILLING_BYTE);

  const std::vector<std::string> fed = lines.feed(text);
  const std::optional<std::string> last = lines.finish();

  EXPECT_EQ(fed, (std::vector<std::string>{
                     std::string(LONGEST_RELAYED_LINE, FILLING_BYTE)}));
  EXPECT_EQ(last.value_or(std::string{}), OVERLONG_LINE_REST);
}

/**
 * Given every line fed ended
 * When the output closes
 * Then nothing is left
 */
TEST(ChildOutputLines, FinishesWithNothingWhenAllLinesEnded) {
  ChildOutputLines lines;
  static_cast<void>(lines.feed(ENDED_LINE));

  EXPECT_FALSE(lines.finish().has_value());
}

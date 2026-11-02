#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "ItemListConstants.hpp"
#include "ItemRow.hpp"
#include "LeaderboardPage.hpp"
#include "LeaderboardPresentation.hpp"
#include "LeaderboardPresentationFixtures.hpp"
#include "LeaderboardPresentationTestConstants.hpp"
#include "LeaderboardScreenConstants.hpp"

using rtype::client::dateTextOf;
using rtype::client::EMPTY_LEADERBOARD_MESSAGE;
using rtype::client::filterOf;
using rtype::client::filterSummaryOf;
using rtype::client::FIRE_GOLD;
using rtype::client::ITEM_TAG_GAP;
using rtype::client::ITEM_TAG_PADDING;
using rtype::client::ItemRow;
using rtype::client::itemRowsOf;
using rtype::client::LEADERBOARD_BRONZE;
using rtype::client::LEADERBOARD_COLUMNS;
using rtype::client::LEADERBOARD_LIST;
using rtype::client::LEADERBOARD_OWN_ROW_COLOR;
using rtype::client::LEADERBOARD_RANK_COLOR;
using rtype::client::LeaderboardMessage;
using rtype::client::LeaderboardPage;
using rtype::client::messageOf;
using rtype::client::pinnedNoteOf;
using rtype::client::pinnedRowOf;
using rtype::client::STAR_WHITE;
using rtype::client::testing::FIRST_SECOND;
using rtype::client::testing::FIRST_SECOND_TEXT;
using rtype::client::testing::HARD_ONE_PILOT;
using rtype::client::testing::LAST_SECOND;
using rtype::client::testing::LAST_SECOND_TEXT;
using rtype::client::testing::LONGEST_NICKNAME_WIDTH;
using rtype::client::testing::NAME_COLUMN;
using rtype::client::testing::NORMAL_TWO_PILOTS;
using rtype::client::testing::OCTOBER_18_2026_SECONDS;
using rtype::client::testing::OCTOBER_18_2026_TEXT;
using rtype::client::testing::SCORE_COLUMN;
using rtype::client::testing::SEVEN_DIGITS_WIDTH;
using rtype::client::testing::UNKNOWN_TEXT;
using rtype::client::testing::YOU_WIDTH_IN_SMALL_FONT;
using rtype::network::LeaderboardRequest;

/**
 * Given a date in the middle of a month
 * When it is written
 * Then its day, month and year show as a row of the design does
 */
TEST(LeaderboardPresentation, ADateShowsItsDayMonthAndYear) {
  EXPECT_EQ(dateTextOf(OCTOBER_18_2026_SECONDS), OCTOBER_18_2026_TEXT);
}

/**
 * Given the first second a date can tell
 * When it is written
 * Then it shows, its single digit padded
 */
TEST(LeaderboardPresentation, TheFirstSecondIsADate) {
  EXPECT_EQ(dateTextOf(FIRST_SECOND), FIRST_SECOND_TEXT);
}

/**
 * Given the second before the first a date can tell
 * When it is written
 * Then it reads as unknown
 */
TEST(LeaderboardPresentation, BeforeTheFirstSecondIsUnknown) {
  EXPECT_EQ(dateTextOf(FIRST_SECOND - 1), UNKNOWN_TEXT);
}

/**
 * Given the last second a year of two digits tells
 * When it is written
 * Then it shows
 */
TEST(LeaderboardPresentation, TheLastSecondIsADate) {
  EXPECT_EQ(dateTextOf(LAST_SECOND), LAST_SECOND_TEXT);
}

/**
 * Given the second after the last one a year of two digits tells
 * When it is written
 * Then it reads as unknown: a server cannot make the client read outside its
 * tables
 */
TEST(LeaderboardPresentation, PastTheLastSecondIsUnknown) {
  EXPECT_EQ(dateTextOf(LAST_SECOND + 1), UNKNOWN_TEXT);
}

/**
 * Given the three best scores and a fourth
 * When their rows are built
 * Then their ranks read gold, white, bronze, then the plain color
 */
TEST(LeaderboardPresentation, TheTopThreeRanksStandOut) {
  const std::vector<ItemRow> rows = itemRowsOf(answeredPage(
      {entryOf(1, "A"), entryOf(2, "B"), entryOf(3, "C"), entryOf(4, "D")},
      std::nullopt));

  ASSERT_EQ(rows.size(), 4);
  EXPECT_EQ(rows[0].cells[0].color, FIRE_GOLD);
  EXPECT_EQ(rows[1].cells[0].color, STAR_WHITE);
  EXPECT_EQ(rows[2].cells[0].color, LEADERBOARD_BRONZE);
  EXPECT_EQ(rows[3].cells[0].color, LEADERBOARD_RANK_COLOR);
}

/**
 * Given the entry of the player among the best scores
 * When the rows are built
 * Then its row is highlighted and tagged in place, and nothing is pinned
 */
TEST(LeaderboardPresentation, AnOwnEntryAmongTheBestIsHighlightedInPlace) {
  const LeaderboardPage page =
      answeredPage({entryOf(1, "KIRA"), entryOf(2, "ACE")}, entryOf(2, "ACE"));

  const std::vector<ItemRow> rows = itemRowsOf(page);

  ASSERT_EQ(rows.size(), 2);
  EXPECT_EQ(rows[0].highlight, std::nullopt);
  EXPECT_EQ(rows[1].highlight, LEADERBOARD_OWN_ROW_COLOR);
  EXPECT_EQ(rows[1].cells[1].tag, "YOU");
  EXPECT_FALSE(pinnedRowOf(page).has_value());
}

/**
 * Given the entry of the player outside the best scores
 * When the rows are built
 * Then no row of the list is highlighted, and its row is pinned
 */
TEST(LeaderboardPresentation, AnOwnEntryOutsideTheBestIsPinned) {
  const LeaderboardPage page =
      answeredPage({entryOf(1, "KIRA")}, entryOf(42, "ACE"));

  EXPECT_EQ(itemRowsOf(page).at(0).highlight, std::nullopt);
  const std::optional<ItemRow> pinned = pinnedRowOf(page);
  ASSERT_TRUE(pinned.has_value());
  EXPECT_EQ(pinned.value_or(ItemRow{}).cells.at(0).text, "42");
}

/**
 * Given a player without a score
 * When the rows are built
 * Then nothing is pinned
 */
TEST(LeaderboardPresentation, NoOwnEntryPinsNothing) {
  EXPECT_FALSE(pinnedRowOf(answeredPage({entryOf(1, "KIRA")}, std::nullopt))
                   .has_value());
}

/**
 * Given rows and a player without a score
 * When the note under the rows is asked for
 * Then it says the player has no score here
 */
TEST(LeaderboardPresentation, APlayerWithoutAScoreGetsANote) {
  EXPECT_EQ(pinnedNoteOf(answeredPage({entryOf(1, "KIRA")}, std::nullopt)),
            "YOU HAVE NO SCORE IN THIS MODE YET");
}

/**
 * Given rows and a player with a score
 * When the note under the rows is asked for
 * Then there is none
 */
TEST(LeaderboardPresentation, APlayerWithAScoreGetsNoNote) {
  EXPECT_EQ(
      pinnedNoteOf(answeredPage({entryOf(1, "KIRA")}, entryOf(1, "KIRA"))), "");
}

/**
 * Given no rows at all
 * When the note under the rows is asked for
 * Then there is none: the message of the panel says it
 */
TEST(LeaderboardPresentation, NoRowsGetNoNote) {
  EXPECT_EQ(pinnedNoteOf(answeredPage({}, std::nullopt)), "");
}

/**
 * Given the longest nickname of the player, a score of 7 digits
 * When its row is laid out on the columns of the screen
 * Then YOU stops before the score
 */
TEST(LeaderboardPresentation, TheLongestRowKeepsItsCellsApart) {
  const int left = LEADERBOARD_LIST.topLeft.x;
  const int tagEnd = left + LEADERBOARD_COLUMNS.at(NAME_COLUMN).anchor +
                     LONGEST_NICKNAME_WIDTH + ITEM_TAG_GAP +
                     YOU_WIDTH_IN_SMALL_FONT + ITEM_TAG_PADDING;
  const int scoreStart =
      left + LEADERBOARD_COLUMNS.at(SCORE_COLUMN).anchor - SEVEN_DIGITS_WIDTH;

  EXPECT_LE(tagEnd, scoreStart);
}

/**
 * Given a page answered without an entry
 * When the panel asks what to say
 * Then it says this mode has no score yet
 */
TEST(LeaderboardPresentation, AnEmptyAnswerSaysSo) {
  const std::optional message = messageOf(answeredPage({}, std::nullopt));

  ASSERT_TRUE(message.has_value());
  EXPECT_STREQ(message.value_or(LeaderboardMessage{}).line,
               EMPTY_LEADERBOARD_MESSAGE.line);
}

/**
 * Given a choice of the player
 * When it is turned into a filter
 * Then it names the difficulty as the waves file does, and counts the pilots
 * from one
 */
TEST(LeaderboardPresentation, AChoiceNamesItsDifficultyAndItsPilots) {
  EXPECT_EQ(filterOf(NORMAL_TWO_PILOTS),
            (LeaderboardRequest{.playerCount = 2, .difficulty = "normal"}));
}

/**
 * Given a choice of one pilot
 * When it is summed up
 * Then it reads in the singular
 */
TEST(LeaderboardPresentation, OnePilotReadsInTheSingular) {
  EXPECT_EQ(filterSummaryOf(HARD_ONE_PILOT), "HARD GAMES, 1 PILOT");
}

/**
 * Given a choice of two pilots
 * When it is summed up
 * Then it reads in the plural
 */
TEST(LeaderboardPresentation, SeveralPilotsReadInThePlural) {
  EXPECT_EQ(filterSummaryOf(NORMAL_TWO_PILOTS), "NORMAL GAMES, 2 PILOTS");
}

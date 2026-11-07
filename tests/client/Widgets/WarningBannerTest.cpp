#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <optional>
#include <vector>
#include "FlashPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ScreenTestConstants.hpp"
#include "WarningBanner.hpp"
#include "WarningBannerFixtures.hpp"
#include "WarningBannerTestConstants.hpp"

using rtype::client::FlashPreference;
using rtype::client::warningBannerAt;
using rtype::client::WarningBannerLook;

/**
 * Given the boss stage just started
 * When the screen is read one nanosecond before 0.6 s
 * Then nothing shows: the music is silent first
 */
TEST(WarningBanner, NothingShowsDuringTheSilence) {
  EXPECT_EQ(warningBannerAt(WARNING_SHOWS - ONE_TICK, FlashPreference::Full),
            WarningBannerLook{});
}

/**
 * Given the WARNING screen starting at 0.6 s
 * When it is read at 0.6 s, one nanosecond before 0.66 s, then at 0.66 s
 * Then the word shows one letter, still one, then two
 */
TEST(WarningBanner, TheWordIsTypedOneLetterEverySixtyMilliseconds) {
  EXPECT_EQ(
      warningBannerAt(FIRST_LETTER_TIME, FlashPreference::Full).wordLetters,
      1U);
  EXPECT_EQ(
      warningBannerAt(SECOND_LETTER_TIME - ONE_TICK, FlashPreference::Full)
          .wordLetters,
      1U);
  EXPECT_EQ(
      warningBannerAt(SECOND_LETTER_TIME, FlashPreference::Full).wordLetters,
      2U);
}

/**
 * Given the WARNING screen
 * When it is read one nanosecond before 0.96 s, then at 0.96 s
 * Then the word shows six letters, then all seven
 */
TEST(WarningBanner, TheWholeWordIsWrittenSixLetterTimesAfterTheFirst) {
  EXPECT_EQ(warningBannerAt(WHOLE_WORD_TIME - ONE_TICK, FlashPreference::Full)
                .wordLetters,
            LETTERS_OF_WARNING - 1);
  EXPECT_EQ(warningBannerAt(WHOLE_WORD_TIME, FlashPreference::Full).wordLetters,
            LETTERS_OF_WARNING);
}

/**
 * Given the WARNING screen
 * When it is read at 0.6 s, then at 0.95 s
 * Then its band is shut, then open 30 rows on each side of its middle
 */
TEST(WarningBanner, TheBandOpensInThreeHundredFiftyMilliseconds) {
  EXPECT_EQ(
      warningBannerAt(WARNING_SHOWS, FlashPreference::Full).bandHalfHeight, 0);
  EXPECT_EQ(warningBannerAt(BAND_OPEN, FlashPreference::Full).bandHalfHeight,
            OPEN_BAND_HALF_HEIGHT);
}

/**
 * Given the WARNING screen
 * When it is read at 0.6 s, then at 0.95 s
 * Then its tapes are a whole width away, then in their place
 */
TEST(WarningBanner, TheTapesSlideInWhileTheBandOpens) {
  EXPECT_EQ(warningBannerAt(WARNING_SHOWS, FlashPreference::Full).tapeShift,
            WHOLE_TAPE_SHIFT);
  EXPECT_EQ(warningBannerAt(BAND_OPEN, FlashPreference::Full).tapeShift, 0);
}

/**
 * Given the WARNING screen
 * When it is read at 3.4 s, then one nanosecond before 3.8 s
 * Then its band is still open, then shut
 */
TEST(WarningBanner, TheBandClosesInTheLastFourHundredMilliseconds) {
  EXPECT_EQ(warningBannerAt(BAND_STARTS_CLOSING, FlashPreference::Full)
                .bandHalfHeight,
            OPEN_BAND_HALF_HEIGHT);
  EXPECT_EQ(warningBannerAt(WARNING_GONE - ONE_TICK, FlashPreference::Full)
                .bandHalfHeight,
            0);
}

/**
 * Given the WARNING screen
 * When it is read one nanosecond before 3.4 s, then at 3.4 s
 * Then the whole word shows, then no letter of the word or of its line
 */
TEST(WarningBanner, TheTextLeavesWhenTheBandStartsClosing) {
  EXPECT_EQ(
      warningBannerAt(BAND_STARTS_CLOSING - ONE_TICK, FlashPreference::Full)
          .wordLetters,
      LETTERS_OF_WARNING);
  const WarningBannerLook closing =
      warningBannerAt(BAND_STARTS_CLOSING, FlashPreference::Full);
  EXPECT_EQ(closing.wordLetters, 0U);
  EXPECT_EQ(closing.subtitleLetters, 0U);
}

/**
 * Given the WARNING screen
 * When it is read at 3.8 s
 * Then nothing shows
 */
TEST(WarningBanner, TheScreenIsGoneAtThreePointEightSeconds) {
  EXPECT_EQ(warningBannerAt(WARNING_GONE, FlashPreference::Full),
            WarningBannerLook{});
}

/**
 * Given the WARNING screen
 * When it is read at 0.6 s, then at 1.1 s
 * Then the word is torn 4 pixels sideways, then not at all
 */
TEST(WarningBanner, TheWordTearsSidewaysAtFirst) {
  EXPECT_EQ(warningBannerAt(WARNING_SHOWS, FlashPreference::Full).tear,
            FIRST_TEAR);
  EXPECT_EQ(warningBannerAt(TEAR_OVER, FlashPreference::Full).tear, 0);
}

/**
 * Given the WARNING screen, flashes reduced
 * When it is read at 0.6 s
 * Then the word is not torn
 */
TEST(WarningBanner, WithFlashesReducedTheWordNeverTears) {
  EXPECT_EQ(warningBannerAt(WARNING_SHOWS, FlashPreference::Reduced).tear, 0);
}

/**
 * Given the WARNING screen once open
 * When it is painted
 * Then no fill reaches the status band, the last 24 rows of the picture
 */
TEST(WarningBanner, NothingIsPaintedOnTheStatusBand) {
  const InterfaceCanvasRecord record =
      warningPaintingOf(warningBannerAt(BAND_OPEN, FlashPreference::Full));

  for (const auto& [column, row] : litPixelsOf(record)) {
    EXPECT_LT(row, PLAYFIELD_ROWS_ABOVE_THE_BAND);
  }
}

/**
 * Given the word with one letter, then with every letter, flashes reduced
 * When each is painted
 * Then the first line of the word is written at the same place: a new letter
 * never moves the others
 */
TEST(WarningBanner, ANewLetterNeverMovesTheOthers) {
  const InterfaceCanvasRecord oneLetter = warningPaintingOf(
      warningBannerAt(FIRST_LETTER_TIME, FlashPreference::Reduced));
  const InterfaceCanvasRecord wholeWord = warningPaintingOf(
      warningBannerAt(WHOLE_WORD_TIME, FlashPreference::Reduced));

  EXPECT_EQ(anchorOf(oneLetter, FIRST_LETTER_ON_SCREEN),
            anchorOf(wholeWord, WARNING_ON_SCREEN));
}

/**
 * Given the whole word, flashes reduced
 * When it is painted
 * Then it is written in eight clipped bands, the fourth one on rows 104 to
 * 107 across the picture
 */
TEST(WarningBanner, EachBandOfTheWordIsClippedToItsRows) {
  const std::vector<RecordedWriting> bands =
      clippedWritingsOf(warningPaintingOf(
          warningBannerAt(WHOLE_WORD_TIME, FlashPreference::Reduced)));

  ASSERT_EQ(bands.size(), FIRE_BAND_COUNT);
  EXPECT_EQ(bands.at(FOURTH_FIRE_BAND).style.clip,
            std::optional<sf::IntRect>{FOURTH_BAND_CLIP});
}

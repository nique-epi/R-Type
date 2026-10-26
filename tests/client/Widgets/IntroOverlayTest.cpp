#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include "InterfaceColorConstants.hpp"
#include "IntroOverlayConstants.hpp"
#include "IntroOverlayFixtures.hpp"
#include "IntroOverlayTestConstants.hpp"
#include "IntroTimeline.hpp"
#include "IntroTimelineConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::closeupStartOf;
using rtype::client::INK;
using rtype::client::INTRO_BAND_OPENING;
using rtype::client::INTRO_HINT;
using rtype::client::INTRO_SLAM;
using rtype::client::INTRO_SLAM_TIME;
using rtype::client::PLAYER_LIGHT_PINK;
using rtype::client::PLAYER_PINK;
using rtype::client::testing::A_MOMENT;
using rtype::client::testing::BAND_SIZE;
using rtype::client::testing::BAND_TEXT;
using rtype::client::testing::BEFORE_THE_HINT;
using rtype::client::testing::BOTTOM_BAND_TOP_LEFT;
using rtype::client::testing::CREDIT_AT_ITS_START;
using rtype::client::testing::DURING_THE_BANDS;
using rtype::client::testing::HINT_ON_THE_BAND;
using rtype::client::testing::HINT_ON_THE_FOOTER;
using rtype::client::testing::INTO_A_CLOSEUP;
using rtype::client::testing::INTO_THE_CREDIT;
using rtype::client::testing::LOGO_AND_ITS_TRAIL;
using rtype::client::testing::LOGO_ON_THE_TITLE;
using rtype::client::testing::LUKAS_UNDERLINE;
using rtype::client::testing::ONE_IMAGE;
using rtype::client::testing::SECOND_PILOT_NAME;
using rtype::client::testing::UNDERLINE_TOP_LEFT;
using ::testing::Contains;

/**
 * Given the first beat
 * When the overlay is painted early in it
 * Then the credit is being typed on the bottom band
 */
TEST(IntroOverlay, TheCreditIsTypedOnTheBottomBand) {
  const InterfaceCanvasRecord record = overlayAt(INTO_THE_CREDIT);

  EXPECT_EQ(anchorOf(record, CREDIT_AT_ITS_START), BAND_TEXT);
}

/**
 * Given the close-up of the second pilot
 * When the overlay is painted well into it
 * Then its nickname is written on the bottom band in its light shade
 */
TEST(IntroOverlay, ThePilotOfACloseUpIsNamedOnTheBottomBand) {
  const InterfaceCanvasRecord record =
      overlayAt(closeupStartOf(1) + INTO_A_CLOSEUP);

  const RecordedWriting* name = writingOf(record, SECOND_PILOT_NAME);
  ASSERT_NE(name, nullptr);
  EXPECT_EQ(name->anchor, BAND_TEXT);
  EXPECT_EQ(name->style.color, PLAYER_LIGHT_PINK);
}

/**
 * Given the close-up of the second pilot
 * When the overlay is painted once the name is typed
 * Then the name is underlined in the color of the ship, as long as it is
 */
TEST(IntroOverlay, TheNameIsUnderlinedInTheColorOfItsShip) {
  const InterfaceCanvasRecord record =
      overlayAt(closeupStartOf(1) + INTO_A_CLOSEUP);

  EXPECT_THAT(record.fills,
              Contains(RecordedFill{
                  .area = sf::IntRect(UNDERLINE_TOP_LEFT, LUKAS_UNDERLINE),
                  .color = PLAYER_PINK}));
}

/**
 * Given the scene before the slam
 * When the overlay is painted
 * Then both bands close the top and the bottom of the picture
 */
TEST(IntroOverlay, TheBandsCloseTheScreenBeforeTheSlam) {
  const InterfaceCanvasRecord record = overlayAt(DURING_THE_BANDS);

  EXPECT_THAT(record.fills,
              Contains(RecordedFill{.area = sf::IntRect({0, 0}, BAND_SIZE),
                                    .color = INK}));
  EXPECT_THAT(
      record.fills,
      Contains(RecordedFill{
          .area = sf::IntRect(BOTTOM_BAND_TOP_LEFT, BAND_SIZE), .color = INK}));
}

/**
 * Given the bands open once the slam started
 * When the overlay is painted after their opening
 * Then nothing is painted in ink
 */
TEST(IntroOverlay, TheBandsAreGoneOnceOpen) {
  const InterfaceCanvasRecord record =
      overlayAt(INTRO_SLAM + INTRO_BAND_OPENING + A_MOMENT);

  EXPECT_TRUE(std::ranges::none_of(record.fills, [](const RecordedFill& fill) {
    return fill.color == INK;
  }));
}

/**
 * Given the first second of the intro
 * When the overlay is painted just before PRESS ANY KEY shows
 * Then it is not written yet
 */
TEST(IntroOverlay, PressAnyKeyWaitsForItsMoment) {
  EXPECT_EQ(writingOf(overlayAt(BEFORE_THE_HINT), INTRO_HINT), nullptr);
}

/**
 * Given the bands still closed
 * When the overlay is painted after PRESS ANY KEY showed
 * Then it lies on the bottom band, right-aligned at the safe margin
 */
TEST(IntroOverlay, PressAnyKeyLiesOnTheBottomBand) {
  EXPECT_EQ(anchorOf(overlayAt(DURING_THE_BANDS), INTRO_HINT),
            HINT_ON_THE_BAND);
}

/**
 * Given the bands open
 * When the overlay is painted
 * Then PRESS ANY KEY lies on the footer row, where the title writes its
 * copyright
 */
TEST(IntroOverlay, PressAnyKeyMovesToTheFooterOnceTheBandsOpen) {
  EXPECT_EQ(anchorOf(overlayAt(INTRO_SLAM + INTRO_BAND_OPENING + A_MOMENT),
                     INTRO_HINT),
            HINT_ON_THE_FOOTER);
}

/**
 * Given the slam of the logo
 * When the overlay is painted once it landed
 * Then the logo lies where the title screen shows it
 */
TEST(IntroOverlay, TheLogoLandsWhereTheTitleShowsIt) {
  const InterfaceCanvasRecord record =
      overlayAt(INTRO_SLAM + INTRO_SLAM_TIME + A_MOMENT);

  ASSERT_EQ(record.images.size(), ONE_IMAGE);
  EXPECT_EQ(record.images.front().topLeft, LOGO_ON_THE_TITLE);
}

/**
 * Given the slam of the logo
 * When the overlay is painted while the logo flies
 * Then it is laid with its three tinted copies
 */
TEST(IntroOverlay, TheLogoFliesWithItsTrail) {
  const InterfaceCanvasRecord record = overlayAt(INTRO_SLAM + A_MOMENT);

  EXPECT_EQ(record.images.size(), LOGO_AND_ITS_TRAIL);
}

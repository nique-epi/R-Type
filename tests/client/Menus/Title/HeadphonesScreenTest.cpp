#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include "HeadphonesScreenFixture.hpp"
#include "HeadphonesScreenTestConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "KeyPress.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ScreenInputs.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::INK;
using rtype::client::INTERFACE_AREA;
using rtype::client::testing::FOR_THE_BEST_EXPERIENCE;
using rtype::client::testing::HALFWAY_INK_ALPHA;
using rtype::client::testing::HALFWAY_INTO_THE_FADE_OUT;
using rtype::client::testing::HALFWAY_INTO_THE_HEADPHONES_FADE_IN;
using rtype::client::testing::HEADPHONES_CLICK_PIXEL;
using rtype::client::testing::HEADPHONES_LINE_PLACE;
using rtype::client::testing::HEADPHONES_LOGO_AREA;
using rtype::client::testing::HEADPHONES_RECOMMENDED;
using rtype::client::testing::HEADPHONES_SMALL_LINE_PLACE;
using rtype::client::testing::HELD_PAST_THE_INTRO;
using rtype::client::testing::IN_THE_LAST_INK;
using rtype::client::testing::JUST_BEFORE_A_HALF_LEAVE_ENDS;
using rtype::client::testing::JUST_BEFORE_A_LEAVE_ENDS;
using rtype::client::testing::JUST_BEFORE_THE_INTRO;
using rtype::client::testing::MOST_OF_THE_HOLD;
using rtype::client::testing::PAST_THE_HEADPHONES_FADE_IN;
using rtype::client::testing::PAST_WHERE_A_LEAVE_ENDS;
using rtype::client::testing::TO_THE_END_OF_THE_INK;
using rtype::client::testing::WHILE_THE_HEADPHONES_STAY;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given HEADPHONES RECOMMENDED just built
 * When nothing happens
 * Then its picture ends under the whole ink
 */
TEST_F(HeadphonesScreenFixture, OpensOnTheWholeInk) {
  EXPECT_EQ(record().fills.back(),
            (RecordedFill{.area = INTERFACE_AREA, .color = INK}));
}

/**
 * Given HEADPHONES RECOMMENDED
 * When its fade in is over
 * Then only its ground of ink covers the whole picture
 */
TEST_F(HeadphonesScreenFixture, IsWhollyLitOnceFadedIn) {
  advanceFrameByFrame(screen(), PAST_THE_HEADPHONES_FADE_IN);

  EXPECT_EQ(
      std::ranges::count(record().fills, INTERFACE_AREA, &RecordedFill::area),
      1);
}

/**
 * Given HEADPHONES RECOMMENDED faded in
 * When most of the time it stays passes
 * Then it paints nothing more: its picture does not change
 */
TEST_F(HeadphonesScreenFixture, RepaintsNothingWhileItStays) {
  advanceFrameByFrame(screen(), PAST_THE_HEADPHONES_FADE_IN);
  const int displayedOnceFadedIn = record().displayCount;

  advanceFrameByFrame(screen(), MOST_OF_THE_HOLD);

  EXPECT_EQ(record().displayCount, displayedOnceFadedIn);
}

/**
 * Given HEADPHONES RECOMMENDED
 * When it is painted
 * Then the logo and its two lines lie where the mockup places them
 */
TEST_F(HeadphonesScreenFixture, CentersTheLogoAndItsTwoLines) {
  EXPECT_EQ(anchorOf(record(), HEADPHONES_RECOMMENDED), HEADPHONES_LINE_PLACE);
  EXPECT_EQ(anchorOf(record(), FOR_THE_BEST_EXPERIENCE),
            HEADPHONES_SMALL_LINE_PLACE);
  EXPECT_EQ(plottedBounds(), HEADPHONES_LOGO_AREA);
}

/**
 * Given HEADPHONES RECOMMENDED left alone
 * When its 3.6 s pass, then more time
 * Then the intro replaces it once, at 3.6 s and not before
 */
TEST_F(HeadphonesScreenFixture, GivesWayToTheIntroOnceItsTimeIsUp) {
  advanceFrameByFrame(screen(), JUST_BEFORE_THE_INTRO);
  ASSERT_THAT(navigator().calls(), IsEmpty());

  advanceFrameByFrame(screen(), ONE_FRAME);
  advanceFrameByFrame(screen(), PAST_THE_HEADPHONES_FADE_IN);

  EXPECT_THAT(navigator().calls(), ElementsAre(introAsked()));
}

/**
 * Given HEADPHONES RECOMMENDED while it stays
 * When a key is pressed then released
 * Then the intro replaces it a fade out and the ink later, not before
 */
TEST_F(HeadphonesScreenFixture, AReleasedKeyLeavesBeforeItsTime) {
  advanceFrameByFrame(screen(), WHILE_THE_HEADPHONES_STAY);
  screen().handleEvent(keyPress(sf::Keyboard::Key::Enter));
  screen().handleEvent(keyRelease(sf::Keyboard::Key::Enter));
  advanceFrameByFrame(screen(), JUST_BEFORE_A_LEAVE_ENDS);
  ASSERT_THAT(navigator().calls(), IsEmpty());

  advanceFrameByFrame(screen(), ONE_FRAME);

  EXPECT_THAT(navigator().calls(), ElementsAre(introAsked()));
}

/**
 * Given HEADPHONES RECOMMENDED while it stays
 * When the mouse clicks
 * Then the intro replaces it a fade out and the ink later, not before
 */
TEST_F(HeadphonesScreenFixture, AClickLeavesBeforeItsTime) {
  advanceFrameByFrame(screen(), WHILE_THE_HEADPHONES_STAY);
  clickInterfacePixel(screen(), HEADPHONES_CLICK_PIXEL);
  advanceFrameByFrame(screen(), JUST_BEFORE_A_LEAVE_ENDS);
  ASSERT_THAT(navigator().calls(), IsEmpty());

  advanceFrameByFrame(screen(), ONE_FRAME);

  EXPECT_THAT(navigator().calls(), ElementsAre(introAsked()));
}

/**
 * Given HEADPHONES RECOMMENDED while it stays
 * When a key is pressed and held past where a leave would have ended
 * Then the screen stays wholly lit: a press alone never starts the fade out
 */
TEST_F(HeadphonesScreenFixture, AHeldKeyKeepsTheScreenLit) {
  advanceFrameByFrame(screen(), WHILE_THE_HEADPHONES_STAY);
  screen().handleEvent(keyPress(sf::Keyboard::Key::Enter));

  advanceFrameByFrame(screen(), PAST_WHERE_A_LEAVE_ENDS);

  EXPECT_EQ(
      std::ranges::count(record().fills, INTERFACE_AREA, &RecordedFill::area),
      1);
}

/**
 * Given HEADPHONES RECOMMENDED while it stays, a key pressed
 * When the window loses the focus, then the key is released
 * Then it keeps its own time: the release it waited for was lost
 */
TEST_F(HeadphonesScreenFixture, AReleaseAfterTheFocusWasLostDoesNotLeave) {
  advanceFrameByFrame(screen(), WHILE_THE_HEADPHONES_STAY);
  screen().handleEvent(keyPress(sf::Keyboard::Key::Enter));
  screen().handleEvent(sf::Event{sf::Event::FocusLost{}});
  screen().handleEvent(keyRelease(sf::Keyboard::Key::Enter));

  advanceFrameByFrame(screen(), PAST_WHERE_A_LEAVE_ENDS);

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given HEADPHONES RECOMMENDED halfway into its fade in
 * When a key is pressed then released
 * Then it fades out from half its light: the intro comes at 1 s, not 1.3 s
 */
TEST_F(HeadphonesScreenFixture, AReleaseInTheFadeInLeavesFromItsLight) {
  screen().update(HALFWAY_INTO_THE_HEADPHONES_FADE_IN);
  screen().handleEvent(keyPress(sf::Keyboard::Key::Enter));
  screen().handleEvent(keyRelease(sf::Keyboard::Key::Enter));
  advanceFrameByFrame(screen(), JUST_BEFORE_A_HALF_LEAVE_ENDS);
  ASSERT_THAT(navigator().calls(), IsEmpty());

  advanceFrameByFrame(screen(), ONE_FRAME);

  EXPECT_THAT(navigator().calls(), ElementsAre(introAsked()));
}

/**
 * Given HEADPHONES RECOMMENDED while it stays, a key pressed
 * When the key stays held past the time of the screen, then is released
 * Then the intro waits in the ink until the release: a repeated press would
 * skip it
 */
TEST_F(HeadphonesScreenFixture, AKeyHeldPastItsTimeKeepsTheIntroWaiting) {
  advanceFrameByFrame(screen(), WHILE_THE_HEADPHONES_STAY);
  screen().handleEvent(keyPress(sf::Keyboard::Key::Enter));
  advanceFrameByFrame(screen(), HELD_PAST_THE_INTRO);
  ASSERT_THAT(navigator().calls(), IsEmpty());

  screen().handleEvent(keyRelease(sf::Keyboard::Key::Enter));
  advanceFrameByFrame(screen(), ONE_FRAME);

  EXPECT_THAT(navigator().calls(), ElementsAre(introAsked()));
}

/**
 * Given HEADPHONES RECOMMENDED left alone
 * When it is halfway into its fade out
 * Then the ink covers its picture at half its opacity
 */
TEST_F(HeadphonesScreenFixture, FadesOutIntoTheInk) {
  advanceFrameByFrame(screen(), HALFWAY_INTO_THE_FADE_OUT);

  EXPECT_EQ(record().fills.back(),
            (RecordedFill{
                .area = INTERFACE_AREA,
                .color = sf::Color(INK.r, INK.g, INK.b, HALFWAY_INK_ALPHA)}));
}

/**
 * Given HEADPHONES RECOMMENDED in the ink after its fade out
 * When the mouse clicks
 * Then the intro still comes when it was due, not a fade out later
 */
TEST_F(HeadphonesScreenFixture, AClickInTheLastInkDoesNotDelayTheIntro) {
  advanceFrameByFrame(screen(), IN_THE_LAST_INK);
  clickInterfacePixel(screen(), HEADPHONES_CLICK_PIXEL);

  advanceFrameByFrame(screen(), TO_THE_END_OF_THE_INK);

  EXPECT_THAT(navigator().calls(), ElementsAre(introAsked()));
}

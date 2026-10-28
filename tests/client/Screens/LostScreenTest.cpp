#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include "ConnectionPhase.hpp"
#include "KeyPress.hpp"
#include "LostScreenFixture.hpp"
#include "LostScreenTestConstants.hpp"
#include "MotionPreference.hpp"
#include "NoSignalCardTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ScreenId.hpp"
#include "ScreenInputs.hpp"
#include "ScreenTestConstants.hpp"
#include "SoundConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::CLICK_SOUND_ID;
using rtype::client::ConnectionPhase;
using rtype::client::MotionPreference;
using rtype::client::PRESS_DURATION;
using rtype::client::ScreenId;
using testing::ElementsAre;
using testing::IsEmpty;

/**
 * Given the NO SIGNAL screen
 * When the stack asks what it hides
 * Then it hides the screens below
 */
TEST_F(LostScreenFixture, HidesTheScreensBelow) {
  EXPECT_TRUE(screen().hidesScreensBelow());
}

/**
 * Given the screen that has just been built
 * When nothing else happens
 * Then the card is not painted: the screen turns off first
 */
TEST_F(LostScreenFixture, ShowsNoCardWhileTheScreenTurnsOff) {
  EXPECT_EQ(writingOf(canvas(), NO_SIGNAL_WORDS), nullptr);
}

/**
 * Given the screen that has just been built
 * When the screen has turned off
 * Then the card says NO SIGNAL
 */
TEST_F(LostScreenFixture, ShowsTheCardOnceTheScreenTurnedOff) {
  advanceFrameByFrame(screen(), SCREEN_TURNED_OFF);

  EXPECT_NE(writingOf(canvas(), NO_SIGNAL_WORDS), nullptr);
}

/**
 * Given the card shown
 * When Enter is pressed and the press has shown
 * Then the title replaces every screen and the connection screen opens over
 * it
 */
TEST_F(LostScreenFixture, EnterOpensTheConnectionOverTheTitle) {
  advanceFrameByFrame(screen(), SCREEN_TURNED_OFF);

  tapKey(screen(), sf::Keyboard::Key::Enter);
  advanceFrameByFrame(screen(), PRESS_DURATION);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::Title},
                          NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::Connection}));
}

/**
 * Given the card shown and Enter pressed
 * When the press has not quite shown
 * Then nothing is asked yet
 */
TEST_F(LostScreenFixture, ThePressShowsBeforeTheConnectionIsAsked) {
  advanceFrameByFrame(screen(), SCREEN_TURNED_OFF);

  tapKey(screen(), sf::Keyboard::Key::Enter);
  advanceFrameByFrame(screen(), PRESS_DURATION - ONE_TICK);

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given the screen turning off
 * When Enter is pressed
 * Then the card shows at once and nothing is asked
 */
TEST_F(LostScreenFixture, EnterWhileTheScreenTurnsOffShowsTheCard) {
  tapKey(screen(), sf::Keyboard::Key::Enter);

  EXPECT_NE(writingOf(canvas(), NO_SIGNAL_WORDS), nullptr);
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given the screen turning off
 * When Escape is pressed
 * Then the title replaces every screen
 */
TEST_F(LostScreenFixture, EscapeLeavesForTheTitle) {
  tapKey(screen(), sf::Keyboard::Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::Title}));
}

/**
 * Given the whole call typed
 * When the player clicks it and the press has shown
 * Then the connection screen opens over the title
 */
TEST_F(LostScreenFixture, AClickOnTheCallReconnects) {
  advanceFrameByFrame(screen(), WHOLE_CALL_TYPED);

  clickInterfacePixel(screen(), MIDDLE_OF_THE_CALL);
  advanceFrameByFrame(screen(), PRESS_DURATION);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::Title},
                          NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::Connection}));
}

/**
 * Given the card shown
 * When Enter presses the call
 * Then the click of the menus sounds
 */
TEST_F(LostScreenFixture, PressingTheCallClicks) {
  advanceFrameByFrame(screen(), SCREEN_TURNED_OFF);

  tapKey(screen(), sf::Keyboard::Key::Enter);

  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}

/**
 * Given a player who reduced motion
 * When the screen is built
 * Then it opens on the card with the whole call
 */
TEST_F(LostScreenFixture, ReducedMotionOpensOnTheWholeCall) {
  rebuildFor(ConnectionPhase::Lost, MotionPreference::Reduced);

  EXPECT_NE(writingOf(canvas(), RECONNECT_WORDS), nullptr);
}

/**
 * Given a session the server closed
 * When the card shows
 * Then it says that the server shut down
 */
TEST_F(LostScreenFixture, AClosedSessionSaysTheServerShutDown) {
  rebuildFor(ConnectionPhase::Closed, MotionPreference::Reduced);

  EXPECT_NE(writingOf(canvas(), CLOSED_WORDS), nullptr);
}

/**
 * Given the screen turning off
 * When Enter is pressed and held, the system repeating its press
 * Then the card shows and nothing is asked: a key held down acts once
 */
TEST_F(LostScreenFixture, AHeldEnterActsOnce) {
  screen().handleEvent(keyPress(sf::Keyboard::Key::Enter));

  screen().handleEvent(keyPress(sf::Keyboard::Key::Enter));
  advanceFrameByFrame(screen(), PRESS_DURATION);

  EXPECT_NE(writingOf(canvas(), NO_SIGNAL_WORDS), nullptr);
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given the screen turning off
 * When the player clicks a corner of the picture, far from the call
 * Then the card shows at once
 */
TEST_F(LostScreenFixture, AClickAnywhereWhileTheScreenTurnsOffShowsTheCard) {
  clickInterfacePixel(screen(), CORNER_OF_THE_PICTURE);

  EXPECT_NE(writingOf(canvas(), NO_SIGNAL_WORDS), nullptr);
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

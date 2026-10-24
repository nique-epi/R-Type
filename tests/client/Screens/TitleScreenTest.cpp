#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include "BlankRenderTarget.hpp"
#include "KeyPress.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "TitleInTheStackFixture.hpp"
#include "TitleScreenFixture.hpp"
#include "TitleScreenTestConstants.hpp"

using rtype::client::LOGO_ID;
using rtype::client::ScreenId;
using rtype::client::testing::LOGO_TOP_LEFT;
using rtype::client::testing::PLAY_BAR_MIDDLE;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given the title screen just built
 * When nothing happened yet
 * Then its picture already holds the logo, at its place on the mockup
 */
TEST_F(TitleScreenFixture, TheFirstFrameAlreadyShowsTheLogo) {
  ASSERT_FALSE(canvas().images.empty());
  EXPECT_EQ(canvas().images.front().assetId, LOGO_ID);
  EXPECT_EQ(canvas().images.front().topLeft, LOGO_TOP_LEFT);
}

/**
 * Given the title screen, PLAY selected
 * When Enter is pressed and released, and the press lasts
 * Then the connection screen is asked for over the title
 */
TEST_F(TitleScreenFixture, PlayOpensTheConnectionScreen) {
  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::Connection}));
}

/**
 * Given the title screen, PLAY selected
 * When the down arrow selects SETTINGS and Enter activates it
 * Then the options are asked for over the title
 */
TEST_F(TitleScreenFixture, SettingsOpensTheOptionsOverTheTitle) {
  title().handleEvent(keyPress(sf::Keyboard::Key::Down));

  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::Options}));
}

/**
 * Given the title screen, PLAY selected
 * When the up arrow wraps to QUIT and Enter activates it
 * Then the quit confirmation is asked for over the title
 */
TEST_F(TitleScreenFixture, QuitOpensTheConfirmation) {
  title().handleEvent(keyPress(sf::Keyboard::Key::Up));

  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::QuitConfirmation}));
}

/**
 * Given the title screen, PLAY selected
 * When Escape is pressed and released
 * Then the quit confirmation is asked for at once
 */
TEST_F(TitleScreenFixture, EscapeOpensTheConfirmation) {
  title().handleEvent(keyPress(sf::Keyboard::Key::Escape));
  title().handleEvent(keyRelease(sf::Keyboard::Key::Escape));

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::QuitConfirmation}));
}

/**
 * Given the title screen
 * When it is drawn
 * Then its picture is laid once
 */
TEST_F(TitleScreenFixture, DrawingLaysItsPicture) {
  BlankRenderTarget target;

  title().draw(target);

  EXPECT_EQ(canvas().showCount, 1);
}

/**
 * Given the options over the title
 * When a frame is drawn
 * Then the picture of the title is not laid
 */
TEST_F(TitleInTheStackFixture, TheOptionsHideTheTitle) {
  BlankRenderTarget target;
  open(ScreenId::Options);

  screens().draw(target);

  EXPECT_EQ(titleCanvas().showCount, 0);
}

/**
 * Given the quit confirmation over the title
 * When a frame is drawn
 * Then the picture of the title is laid under the dialog, once
 */
TEST_F(TitleInTheStackFixture, TheQuitConfirmationLeavesTheTitleShown) {
  BlankRenderTarget target;
  open(ScreenId::QuitConfirmation);

  screens().draw(target);

  EXPECT_EQ(titleCanvas().showCount, 1);
}

/**
 * Given the options over the title
 * When the left button is pressed and released where the bar of PLAY lies on
 * the title, and the press lasts
 * Then the title asks for nothing
 */
TEST_F(TitleInTheStackFixture,
       AClickInTheOptionsNeverPressesAButtonOfTheTitle) {
  open(ScreenId::Options);

  click(PLAY_BAR_MIDDLE);

  EXPECT_THAT(titleNavigator().calls(), IsEmpty());
}

/**
 * Given the options opened over the title, then closed
 * When the left button is pressed and released on the bar of PLAY, and the
 * press lasts
 * Then the title asks for the connection screen
 */
TEST_F(TitleInTheStackFixture, OnceTheOptionsAreClosedAClickReachesTheTitle) {
  open(ScreenId::Options);
  closeTheScreenOnTop();

  click(PLAY_BAR_MIDDLE);

  EXPECT_THAT(titleNavigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::Connection}));
}

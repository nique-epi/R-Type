#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "BlankRenderTarget.hpp"
#include "EntitySpriteConstants.hpp"
#include "KeyPress.hpp"
#include "MenuScreenIdConstants.hpp"
#include "MotionPreference.hpp"
#include "PlayerSettings.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenConstants.hpp"
#include "SoundConstants.hpp"
#include "SquadronStart.hpp"
#include "SquadronTestConstants.hpp"
#include "TitleInTheStackFixture.hpp"
#include "TitleScreen.hpp"
#include "TitleScreenFixture.hpp"
#include "TitleScreenParts.hpp"
#include "TitleScreenTestConstants.hpp"

using rtype::client::CLICK_SOUND_ID;
using rtype::client::CONNECTION_SCREEN;
using rtype::client::ENTITY_SHEET_ID;
using rtype::client::LOGO_ID;
using rtype::client::MotionPreference;
using rtype::client::OPTIONS_SCREEN;
using rtype::client::PlayerSettings;
using rtype::client::QUIT_CONFIRMATION_SCREEN;
using rtype::client::SquadronStart;
using rtype::client::TitleScreen;
using rtype::client::TitleScreenParts;
using rtype::client::testing::LOGO_TOP_LEFT;
using rtype::client::testing::PLAY_BAR_MIDDLE;
using rtype::client::testing::SHIP_SIZE;
using rtype::client::testing::SHIPS_IN_PLACE_ON_THE_TITLE;
using rtype::client::testing::TITLE_SEED;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::UnorderedElementsAreArray;

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
 * Given the title screen the intro gives way to, its squadron in place
 * When its first frame is painted
 * Then the four ships already sit in their places in the box, where the
 * intro leaves them
 */
TEST(TitleScreen, ASquadronInPlaceShowsItsShipsOnTheFirstFrame) {
  RecordingNavigator navigator;
  RecordingSoundPlayer sounds;
  InterfaceCanvasRecord canvas;
  const PlayerSettings settings;

  const TitleScreen title(
      TitleScreenParts{.navigator = navigator,
                       .sounds = sounds,
                       .settings = settings,
                       .playfieldPointAt = samePlayfieldPoint,
                       .motion = MotionPreference::Full,
                       .seed = TITLE_SEED,
                       .squadronStart = SquadronStart::InPlace},
      std::make_unique<RecordingInterfaceCanvas>(canvas));

  EXPECT_THAT(untintedImagesOf(canvas, ENTITY_SHEET_ID, SHIP_SIZE),
              UnorderedElementsAreArray(SHIPS_IN_PLACE_ON_THE_TITLE));
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
                                        .screen = CONNECTION_SCREEN}));
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
                                        .screen = OPTIONS_SCREEN}));
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
                                        .screen = QUIT_CONFIRMATION_SCREEN}));
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
                                        .screen = QUIT_CONFIRMATION_SCREEN}));
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
  open(OPTIONS_SCREEN);

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
  open(QUIT_CONFIRMATION_SCREEN);

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
  open(OPTIONS_SCREEN);

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
  open(OPTIONS_SCREEN);
  closeTheScreenOnTop();

  click(PLAY_BAR_MIDDLE);

  EXPECT_THAT(titleNavigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = CONNECTION_SCREEN}));
}

/**
 * Given the title screen, PLAY selected
 * When Enter is pressed and released, and the press lasts
 * Then the click of PLAY plays once
 */
TEST_F(TitleScreenFixture, ChoosingPlayPlaysTheClick) {
  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}

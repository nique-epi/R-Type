#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "ConnectionLostInTheStackFixture.hpp"
#include "ConnectionPhase.hpp"
#include "DialogMenusFixture.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingScreen.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "SoundConstants.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

using rtype::client::CLICK_SOUND_ID;
using rtype::client::CLOSED_SERVER_MESSAGE;
using rtype::client::CONNECTION_LOST_HEADING;
using rtype::client::ConnectionPhase;
using rtype::client::DIALOG_MESSAGE_OFFSET;
using rtype::client::DIALOG_SIZE;
using rtype::client::regularTextWidth;
using rtype::client::ScreenId;
using rtype::client::SIDES_PER_AXIS;
using rtype::client::SILENT_SERVER_MESSAGE;
using ::testing::ElementsAre;

/**
 * Given the quit confirmation just opened
 * When Enter is pressed and released, and the press lasts
 * Then NO answers: the dialog asks to close itself
 */
TEST_F(DialogMenusFixture, NoIsChosenAtFirstOnTheQuitConfirmation) {
  openQuitConfirmation();

  tap(sf::Keyboard::Key::Enter);
  letThePressLast();

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the quit confirmation just opened
 * When Escape is pressed and released
 * Then NO answers at once
 */
TEST_F(DialogMenusFixture, EscapeAnswersNoOnTheQuitConfirmation) {
  openQuitConfirmation();

  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the quit confirmation, NO selected
 * When the right arrow selects YES and Enter activates it
 * Then every screen is asked to close, which closes the client
 */
TEST_F(DialogMenusFixture, YesClosesTheClient) {
  openQuitConfirmation();

  press(sf::Keyboard::Key::Right);
  tap(sf::Keyboard::Key::Enter);
  letThePressLast();

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::CloseAll,
                                        .screen = std::nullopt}));
}

/**
 * Given the quit confirmation
 * When Escape answers NO
 * Then the click of NO plays once
 */
TEST_F(DialogMenusFixture, AnsweringTheQuitConfirmationPlaysTheClick) {
  openQuitConfirmation();

  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}

/**
 * Given the dialog of a session the server let go silent
 * When it opens
 * Then it says the connection is lost and the server stopped answering
 */
TEST_F(DialogMenusFixture, ASilentServerStoppedAnswering) {
  openConnectionLost(ConnectionPhase::Lost);

  EXPECT_NE(writingOf(canvas(), CONNECTION_LOST_HEADING), nullptr);
  EXPECT_NE(writingOf(canvas(), "THE SERVER STOPPED ANSWERING."), nullptr);
}

/**
 * Given the dialog of a session the server closed
 * When it opens
 * Then it says the connection is lost and the server shut down
 */
TEST_F(DialogMenusFixture, AClosedServerShutDown) {
  openConnectionLost(ConnectionPhase::Closed);

  EXPECT_NE(writingOf(canvas(), CONNECTION_LOST_HEADING), nullptr);
  EXPECT_NE(writingOf(canvas(), "THE SERVER SHUT DOWN."), nullptr);
}

/**
 * Given either reason of the connection lost dialog
 * When its width is compared with the panel of a dialog
 * Then it stays inside the panel, its margin kept on both sides
 */
TEST(DialogMenus, TheReasonsOfALostConnectionFitTheirPanel) {
  const int widestLine =
      DIALOG_SIZE.x - (SIDES_PER_AXIS * DIALOG_MESSAGE_OFFSET.x);

  EXPECT_LE(regularTextWidth(SILENT_SERVER_MESSAGE), widestLine);
  EXPECT_LE(regularTextWidth(CLOSED_SERVER_MESSAGE), widestLine);
}

/**
 * Given the connection lost dialog just opened
 * When Enter is pressed and released, and the press lasts
 * Then RETRY answers: the dialog closes and the connection screen opens
 */
TEST_F(DialogMenusFixture, RetryIsChosenAtFirstOnALostConnection) {
  openConnectionLost(ConnectionPhase::Lost);

  tap(sf::Keyboard::Key::Enter);
  letThePressLast();

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt},
                          NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::Connection}));
}

/**
 * Given the connection lost dialog, RETRY selected
 * When the right arrow selects OK and Enter activates it
 * Then only the dialog closes, which leaves the title
 */
TEST_F(DialogMenusFixture, OkLeavesTheTitleOnALostConnection) {
  openConnectionLost(ConnectionPhase::Lost);

  press(sf::Keyboard::Key::Right);
  tap(sf::Keyboard::Key::Enter);
  letThePressLast();

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the connection lost dialog just opened
 * When Escape is pressed and released
 * Then OK answers at once
 */
TEST_F(DialogMenusFixture, EscapeAnswersOkOnALostConnection) {
  openConnectionLost(ConnectionPhase::Closed);

  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the connection lost dialog just opened
 * When Enter activates RETRY
 * Then the click of RETRY plays once
 */
TEST_F(DialogMenusFixture, RetryPlaysTheClick) {
  openConnectionLost(ConnectionPhase::Lost);

  tap(sf::Keyboard::Key::Enter);
  letThePressLast();

  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}

/**
 * Given the connection lost dialog over the title, in a real stack
 * When Enter activates RETRY and an event arrives
 * Then the connection screen stands on top, the title still under it
 */
TEST_F(ConnectionLostInTheStackFixture, RetryOpensTheConnectionOverTheTitle) {
  answerWith(sf::Keyboard::Key::Enter);

  sendAnEvent();

  EXPECT_THAT(journal(),
              ElementsAre(JournalEntry{.screen = ScreenId::Connection,
                                       .call = ScreenCall::HandleEvent}));
}

/**
 * Given the connection lost dialog over the title, in a real stack
 * When Escape answers and an event arrives
 * Then the title is the screen on top
 */
TEST_F(ConnectionLostInTheStackFixture, EscapeLeavesTheTitleOnTop) {
  answerWith(sf::Keyboard::Key::Escape);

  sendAnEvent();

  EXPECT_THAT(journal(),
              ElementsAre(JournalEntry{.screen = ScreenId::Title,
                                       .call = ScreenCall::HandleEvent}));
}

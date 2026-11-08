#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "ConnectionLostInTheStackFixture.hpp"
#include "ConnectionPhase.hpp"
#include "DialogMenusFixture.hpp"
#include "MenuScreenIdConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingScreen.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenConstants.hpp"
#include "ServerConnectionRequest.hpp"
#include "SoundConstants.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

using rtype::client::BANNED_MESSAGE;
using rtype::client::CLICK_SOUND_ID;
using rtype::client::CLOSED_SERVER_MESSAGE;
using rtype::client::CONNECTION_AGAIN_SCREEN;
using rtype::client::CONNECTION_LOST_HEADING;
using rtype::client::ConnectionPhase;
using rtype::client::DIALOG_MESSAGE_OFFSET;
using rtype::client::DIALOG_SIZE;
using rtype::client::EXPELLED_MESSAGE;
using rtype::client::regularTextWidth;
using rtype::client::SERVER_MAP_SCREEN;
using rtype::client::SIDES_PER_AXIS;
using rtype::client::SILENT_SERVER_MESSAGE;
using rtype::client::TITLE_SCREEN;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

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
 * Given the question asked before leaving a game, just opened
 * When Enter is pressed and released, and the press lasts
 * Then NO answers: the dialog asks to close itself, back to the options
 */
TEST_F(DialogMenusFixture, NoIsChosenAtFirstOnTheLeaveGameConfirmation) {
  openLeaveGameConfirmation();

  tap(sf::Keyboard::Key::Enter);
  letThePressLast();

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the question asked before leaving a game, just opened
 * When NO is answered with Enter
 * Then the session is left as it is
 */
TEST_F(DialogMenusFixture, NoKeepsTheSessionOfTheGame) {
  openLeaveGameConfirmation();

  tap(sf::Keyboard::Key::Enter);
  letThePressLast();

  EXPECT_THAT(connection().calls(), IsEmpty());
}

/**
 * Given the question asked before leaving a game, just opened
 * When Escape is pressed and released
 * Then NO answers at once
 */
TEST_F(DialogMenusFixture, EscapeAnswersNoOnTheLeaveGameConfirmation) {
  openLeaveGameConfirmation();

  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the question asked before leaving a game, NO selected
 * When the right arrow selects YES and Enter activates it
 * Then the session is left, which tells the server
 */
TEST_F(DialogMenusFixture, YesLeavesTheSessionOfTheGame) {
  openLeaveGameConfirmation();

  press(sf::Keyboard::Key::Right);
  tap(sf::Keyboard::Key::Enter);
  letThePressLast();

  ASSERT_EQ(connection().calls().size(), 1U);
  EXPECT_EQ(connection().calls().front().request,
            ServerConnectionRequest::Cancel);
}

/**
 * Given the question asked before leaving a game, NO selected
 * When the right arrow selects YES and Enter activates it
 * Then the title is asked in place of every screen
 */
TEST_F(DialogMenusFixture, YesPutsTheTitleInPlaceOfTheGame) {
  openLeaveGameConfirmation();

  press(sf::Keyboard::Key::Right);
  tap(sf::Keyboard::Key::Enter);
  letThePressLast();

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = TITLE_SCREEN}));
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
 * Given the dialog of a session the host expelled
 * When it opens
 * Then it says the connection is lost and the host expelled the player
 */
TEST_F(DialogMenusFixture, AnExpelledPilotIsToldTheHostExpelledIt) {
  openConnectionLost(ConnectionPhase::Expelled);

  EXPECT_NE(writingOf(canvas(), CONNECTION_LOST_HEADING), nullptr);
  EXPECT_NE(writingOf(canvas(), "THE HOST EXPELLED YOU."), nullptr);
}

/**
 * Given the dialog of a session the host banned
 * When it opens
 * Then it says the connection is lost and the host banned the player
 */
TEST_F(DialogMenusFixture, ABannedPilotIsToldTheHostBannedIt) {
  openConnectionLost(ConnectionPhase::Banned);

  EXPECT_NE(writingOf(canvas(), CONNECTION_LOST_HEADING), nullptr);
  EXPECT_NE(writingOf(canvas(), "THE HOST BANNED YOU."), nullptr);
}

/**
 * Given each reason of the connection lost dialog
 * When its width is compared with the panel of a dialog
 * Then it stays inside the panel, its margin kept on both sides
 */
TEST(DialogMenus, TheReasonsOfALostConnectionFitTheirPanel) {
  const int widestLine =
      DIALOG_SIZE.x - (SIDES_PER_AXIS * DIALOG_MESSAGE_OFFSET.x);

  EXPECT_LE(regularTextWidth(SILENT_SERVER_MESSAGE), widestLine);
  EXPECT_LE(regularTextWidth(CLOSED_SERVER_MESSAGE), widestLine);
  EXPECT_LE(regularTextWidth(EXPELLED_MESSAGE), widestLine);
  EXPECT_LE(regularTextWidth(BANNED_MESSAGE), widestLine);
}

/**
 * Given the connection lost dialog just opened
 * When Enter is pressed and released, and the press lasts
 * Then RETRY answers: the dialog closes, the server map opens, and the
 * connection screen of the lost game over it
 */
TEST_F(DialogMenusFixture, RetryIsChosenAtFirstOnALostConnection) {
  openConnectionLost(ConnectionPhase::Lost);

  tap(sf::Keyboard::Key::Enter);
  letThePressLast();

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt},
                          NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = SERVER_MAP_SCREEN},
                          NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = CONNECTION_AGAIN_SCREEN}));
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
 * Then the connection screen of the lost game stands on top
 */
TEST_F(ConnectionLostInTheStackFixture,
       RetryOpensTheConnectionOverTheServerMap) {
  answerWith(sf::Keyboard::Key::Enter);

  sendAnEvent();

  EXPECT_THAT(journal(),
              ElementsAre(JournalEntry{.screen = CONNECTION_AGAIN_SCREEN,
                                       .call = ScreenCall::HandleEvent}));
}

/**
 * Given RETRY answered over the title, in a real stack
 * When the connection screen closes, as BACK closes it, and an event arrives
 * Then the server map gets it: BACK from the form leads to the map
 */
TEST_F(ConnectionLostInTheStackFixture,
       BackFromTheRetryFormLeavesTheServerMap) {
  answerWith(sf::Keyboard::Key::Enter);
  closeTheTopScreen();

  sendAnEvent();

  EXPECT_THAT(journal(),
              ElementsAre(JournalEntry{.screen = SERVER_MAP_SCREEN,
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
              ElementsAre(JournalEntry{.screen = TITLE_SCREEN,
                                       .call = ScreenCall::HandleEvent}));
}

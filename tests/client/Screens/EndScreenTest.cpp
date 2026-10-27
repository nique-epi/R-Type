#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <string>
#include <vector>
#include "EndScreenFixture.hpp"
#include "EndScreenTestConstants.hpp"
#include "FlightLogColorConstants.hpp"
#include "FlightLogTextConstants.hpp"
#include "FlightLogTimingConstants.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "ScreenId.hpp"
#include "ScreenTestConstants.hpp"
#include "SoundConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::EXIT_TIME;
using rtype::client::HEAVY_IMPACT_SOUND_ID;
using rtype::client::HOVERED_QUIT_INK;
using rtype::client::MotionPreference;
using rtype::client::PRESS_DURATION;
using rtype::client::PROMPT_TEXT;
using rtype::client::QUIT_TEXT;
using rtype::client::ScreenId;
using rtype::client::testing::ACE_NICKNAME;
using rtype::client::testing::DESIGN_LANDING;
using rtype::client::testing::NOVA_NICKNAME;
using rtype::client::testing::NOWHERE_PIXEL;
using rtype::client::testing::PROMPT_MIDDLE_PIXEL;
using rtype::client::testing::QUIT_MIDDLE_PIXEL;
using rtype::client::testing::STARDUST_NICKNAME;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given the end screen while the book opens
 * When Enter is pressed
 * Then the screen skips to the choices, and nothing is asked
 */
TEST_F(EndScreenFixture, EnterWhileTheBookOpensSkipsToTheChoices) {
  tap(sf::Keyboard::Key::Enter);

  advance(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), PROMPT_TEXT), nullptr);
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given the choices shown
 * When Enter is pressed, and the press and the leaving book have played
 * Then the waiting room after a game is asked in place of every screen, and
 * the server is not left
 */
TEST_F(EndScreenFixture, EnterOpensTheWaitingRoomOnceTheBookLeft) {
  tap(sf::Keyboard::Key::Enter);
  tap(sf::Keyboard::Key::Enter);

  advance(PRESS_DURATION + EXIT_TIME);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::LobbyAfterGame}));
  EXPECT_THAT(connection().calls(), IsEmpty());
}

/**
 * Given the choices shown and Enter pressed
 * When the book has not quite left
 * Then nothing is asked yet
 */
TEST_F(EndScreenFixture, TheBookLeavesBeforeTheWaitingRoomIsAsked) {
  tap(sf::Keyboard::Key::Enter);
  tap(sf::Keyboard::Key::Enter);

  advance(PRESS_DURATION + EXIT_TIME - ONE_TICK);

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given the choices shown
 * When Escape is pressed, and the press and the leaving book have played
 * Then the server is left and the title asked in place of every screen
 */
TEST_F(EndScreenFixture, EscapeLeavesTheServerForTheTitle) {
  tap(sf::Keyboard::Key::Escape);
  tap(sf::Keyboard::Key::Escape);

  advance(PRESS_DURATION + EXIT_TIME);

  ASSERT_FALSE(connection().calls().empty());
  EXPECT_EQ(connection().calls().back().request,
            ServerConnectionCall::Request::Cancel);
  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::Title}));
}

/**
 * Given the end screen while the book opens
 * When a click lands beside the choices, then on QUIT
 * Then the first click skips, the second quits
 */
TEST_F(EndScreenFixture, AClickOnQuitQuits) {
  click(NOWHERE_PIXEL);
  click(QUIT_MIDDLE_PIXEL);

  advance(PRESS_DURATION + EXIT_TIME);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::Title}));
}

/**
 * Given the end screen while the book opens
 * When a click lands beside the choices, then on PRESS ENTER TO REPLAY
 * Then the first click skips, the second asks for the waiting room after a
 * game once the book left
 */
TEST_F(EndScreenFixture, AClickOnTheCallReplays) {
  click(NOWHERE_PIXEL);
  click(PROMPT_MIDDLE_PIXEL);

  advance(PRESS_DURATION + EXIT_TIME);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::LobbyAfterGame}));
}

/**
 * Given Enter pressed to skip the opening, and the window losing the focus
 * before Enter is released
 * When Enter is pressed again
 * Then it presses PLAY AGAIN: the key the window never saw released does not
 * stay held
 */
TEST_F(EndScreenFixture, AKeyHeldWhenTheFocusWasLostActsAgain) {
  screen().handleEvent(keyPress(sf::Keyboard::Key::Enter));
  screen().handleEvent(sf::Event{sf::Event::FocusLost{}});
  screen().handleEvent(keyPress(sf::Keyboard::Key::Enter));

  advance(PRESS_DURATION + EXIT_TIME);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::LobbyAfterGame}));
}

/**
 * Given the end screen while the book opens
 * When Enter is pressed twice without being released, as a held key repeats
 * Then the first press skips and the second does nothing
 */
TEST_F(EndScreenFixture, AHeldEnterActsOnce) {
  screen().handleEvent(keyPress(sf::Keyboard::Key::Enter));
  screen().handleEvent(keyPress(sf::Keyboard::Key::Enter));

  advance(PRESS_DURATION + EXIT_TIME);

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given the choices shown
 * When the pointer moves over QUIT
 * Then QUIT is written in ink, lit, and nothing is pressed
 */
TEST_F(EndScreenFixture, ThePointerLightsQuitWithoutPressingIt) {
  tap(sf::Keyboard::Key::Enter);
  screen().handleEvent(pointerMove(QUIT_MIDDLE_PIXEL * INTERFACE_PIXEL));

  advance(PRESS_DURATION + EXIT_TIME);

  const RecordedWriting* quit = writingOf(canvas(), QUIT_TEXT);
  ASSERT_NE(quit, nullptr);
  EXPECT_EQ(quit->style.color, HOVERED_QUIT_INK);
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given the end screen of the game of three pilots
 * When the choices show
 * Then the names are written best first, from the top
 */
TEST_F(EndScreenFixture, TheRankingNamesThePilotsBestFirst) {
  tap(sf::Keyboard::Key::Enter);

  advance(ONE_FRAME);

  const RecordedWriting* best = writingOf(canvas(), STARDUST_NICKNAME);
  const RecordedWriting* second = writingOf(canvas(), ACE_NICKNAME);
  const RecordedWriting* last = writingOf(canvas(), NOVA_NICKNAME);
  ASSERT_NE(best, nullptr);
  ASSERT_NE(second, nullptr);
  ASSERT_NE(last, nullptr);
  EXPECT_LT(best->anchor.y, second->anchor.y);
  EXPECT_LT(second->anchor.y, last->anchor.y);
}

/**
 * Given a new end screen
 * When it is built
 * Then the taunt it picked is kept for the next end screen
 */
TEST_F(EndScreenFixture, ThePickedTauntIsKeptForTheNextScreen) {
  EXPECT_TRUE(previousTaunt().has_value());
}

/**
 * Given the end screen just opened
 * When the book lands
 * Then its heavy impact plays
 */
TEST_F(EndScreenFixture, TheBookLandingPlaysTheHeavyImpact) {
  advance(DESIGN_LANDING);

  EXPECT_THAT(sounds().played(), Contains(std::string(HEAVY_IMPACT_SOUND_ID)));
}

/**
 * Given a player who asked for less motion
 * When the end screen opens, and Enter is pressed then held for the press
 * Then the choices show at once, and the waiting room is asked without the
 * leaving book
 */
TEST_F(EndScreenFixture, ReducedMotionOpensOnTheChoicesAndLeavesAtOnce) {
  rebuildWith(MotionPreference::Reduced);
  advance(ONE_FRAME);
  ASSERT_NE(writingOf(canvas(), PROMPT_TEXT), nullptr);

  tap(sf::Keyboard::Key::Enter);
  advance(PRESS_DURATION);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::LobbyAfterGame}));
}

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <optional>
#include <string>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "KeyPress.hpp"
#include "LobbyAfterGameFixture.hpp"
#include "LobbyScreen.hpp"
#include "LobbyScreenFixture.hpp"
#include "LobbyScreenFixtures.hpp"
#include "LobbyScreenTestConstants.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenId.hpp"
#include "ScreenTestConstants.hpp"
#include "SoundConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::CLICK_SOUND_ID;
using rtype::client::PRESS_DURATION;
using rtype::client::ScreenId;
using rtype::client::STAR_GOLD;
using rtype::client::testing::ASK_NOT_READY;
using rtype::client::testing::ASK_READY;
using rtype::client::testing::BACK_CHIP_MIDDLE;
using rtype::client::testing::FIRST_BAY_CELL_INSIDE;
using rtype::client::testing::FIRST_BAY_LOCAL_TAG;
using rtype::client::testing::FIRST_BAY_NICKNAME;
using rtype::client::testing::OTHER_NICKNAME;
using rtype::client::testing::OTHER_NUMBER;
using rtype::client::testing::OWN_NICKNAME;
using rtype::client::testing::OWN_NUMBER;
using rtype::client::testing::READY_KEY;
using rtype::client::testing::THIRD_BAY_NICKNAME;
using ::testing::Contains;
using ::testing::ElementsAre;

/**
 * Given the server lists players 1 and 3
 * When a frame goes by
 * Then each nickname is written in the bay of its number
 */
TEST_F(LobbyScreenFixture, EachPilotTakesTheBayOfItsNumber) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false),
        pilot(OTHER_NUMBER, OTHER_NICKNAME, true)});

  EXPECT_EQ(anchorOf(canvas(), OWN_NICKNAME), FIRST_BAY_NICKNAME);
  EXPECT_EQ(anchorOf(canvas(), OTHER_NICKNAME), THIRD_BAY_NICKNAME);
}

/**
 * Given the server lists player 1 alone
 * When a frame goes by
 * Then the three other bays say OPEN BAY
 */
TEST_F(LobbyScreenFixture, APlaceNobodyTookIsAnOpenBay) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  EXPECT_EQ(std::ranges::count(canvas().writings, std::string("OPEN BAY"),
                               &RecordedWriting::text),
            3);
}

/**
 * Given the local player is player 1
 * When a frame goes by
 * Then YOU ends at column 118 of the first bay
 */
TEST_F(LobbyScreenFixture, TheLocalPlayerIsTaggedYou) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  EXPECT_EQ(anchorOf(canvas(), "YOU"), FIRST_BAY_LOCAL_TAG);
}

/**
 * Given the server lists two players
 * When a frame goes by
 * Then the top-right corner says 2 PILOTS
 */
TEST_F(LobbyScreenFixture, ThePilotCountSaysHowManyAreListed) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false),
        pilot(OTHER_NUMBER, OTHER_NICKNAME, false)});

  EXPECT_NE(writingOf(canvas(), "2 PILOTS"), nullptr);
}

/**
 * Given the server lists the local player alone
 * When a frame goes by
 * Then the count is written in the singular
 */
TEST_F(LobbyScreenFixture, ASinglePilotIsCountedInTheSingular) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  EXPECT_NE(writingOf(canvas(), "1 PILOT"), nullptr);
}

/**
 * Given two listed pilots, neither ready
 * When a frame goes by
 * Then the status says 2 PILOTS NOT READY, naming no open place
 */
TEST_F(LobbyScreenFixture, SeveralPilotsNotReadyAreCounted) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false),
        pilot(OTHER_NUMBER, OTHER_NICKNAME, false)});

  EXPECT_NE(writingOf(canvas(), "2 PILOTS NOT READY"), nullptr);
}

/**
 * Given the local player ready and another pilot not ready
 * When a frame goes by
 * Then the status names the other pilot
 */
TEST_F(LobbyScreenFixture, ASinglePilotNotReadyIsNamed) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, true),
        pilot(OTHER_NUMBER, OTHER_NICKNAME, false)});

  EXPECT_NE(writingOf(canvas(), "CAPTAIN_STARDUST IS NOT READY"), nullptr);
}

/**
 * Given another pilot ready and the local player not ready
 * When a frame goes by
 * Then the status says YOU ARE NOT READY
 */
TEST_F(LobbyScreenFixture, TheLocalPlayerAloneNotReadyIsToldSo) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false),
        pilot(OTHER_NUMBER, OTHER_NICKNAME, true)});

  EXPECT_NE(writingOf(canvas(), "YOU ARE NOT READY"), nullptr);
}

/**
 * Given two listed pilots, both ready, two places open
 * When a frame goes by
 * Then the status says the game is launching: open places are not waited for
 */
TEST_F(LobbyScreenFixture, EveryListedPilotReadyLaunches) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, true),
        pilot(OTHER_NUMBER, OTHER_NICKNAME, true)});

  EXPECT_NE(writingOf(canvas(), "ALL READY: LAUNCHING"), nullptr);
}

/**
 * Given the local player not ready
 * When the R key is pressed and released
 * Then the connection is asked to set the player ready
 */
TEST_F(LobbyScreenFixture, TheReadyKeyAsksToBeReady) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  tap(sf::Keyboard::Key::R, READY_KEY);

  EXPECT_THAT(readyRequests(), ElementsAre(ASK_READY));
}

/**
 * Given the local player asked to be ready
 * When the R key is pressed and released
 * Then the connection is asked to set the player not ready
 */
TEST_F(LobbyScreenFixture, TheReadyKeyAsksNotToBeReadyOnceAsked) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});
  connection().setRequestedReady(true);

  tap(sf::Keyboard::Key::R, READY_KEY);

  EXPECT_THAT(readyRequests(), ElementsAre(ASK_NOT_READY));
}

/**
 * Given the local player not ready
 * When the R key is held, the keyboard repeating its press
 * Then the connection is asked once
 */
TEST_F(LobbyScreenFixture, HoldingTheReadyKeyAsksOnce) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  screen().handleEvent(keyPress(sf::Keyboard::Key::R, READY_KEY));
  screen().handleEvent(keyPress(sf::Keyboard::Key::R, READY_KEY));

  EXPECT_THAT(readyRequests(), ElementsAre(ASK_READY));
}

/**
 * Given READY selected
 * When Enter is pressed and released, and the press shows
 * Then the connection is asked to set the player ready
 */
TEST_F(LobbyScreenFixture, TheReadyBarAsksToBeReady) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  tap(sf::Keyboard::Key::Enter, sf::Keyboard::Scan::Enter);
  screen().update(PRESS_DURATION);

  EXPECT_THAT(readyRequests(), ElementsAre(ASK_READY));
}

/**
 * Given the local player asked to be ready
 * When a frame goes by
 * Then the bar reads CANCEL READY
 */
TEST_F(LobbyScreenFixture, TheBarSaysCancelReadyOnceAsked) {
  connection().setRequestedReady(true);

  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  EXPECT_NE(writingOf(canvas(), "CANCEL READY"), nullptr);
}

/**
 * Given the local player asked to be ready, the server still listing it not
 * ready
 * When a frame goes by
 * Then its cell says READY... on gold
 */
TEST_F(LobbyScreenFixture, AnAskTheServerDoesNotShowYetIsWaitedFor) {
  connection().setRequestedReady(true);

  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  EXPECT_NE(writingOf(canvas(), "READY..."), nullptr);
  EXPECT_THAT(canvas().fills,
              Contains(RecordedFill{.area = FIRST_BAY_CELL_INSIDE,
                                    .color = STAR_GOLD}));
}

/**
 * Given the connection learned that the game started
 * When two frames go by
 * Then the game is asked for in place of every screen, once
 */
TEST_F(LobbyScreenFixture, TheGameIsAskedOnceItStarted) {
  connection().setGameStarted(true);

  screen().update(ONE_FRAME);
  screen().update(ONE_FRAME);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::Game}));
}

/**
 * Given the waiting room after a game, alone on the stack
 * When Escape is pressed and released
 * Then the server is left and the title opened in place of every screen
 */
TEST_F(LobbyAfterGameFixture, BackLeavesForTheTitle) {
  tap(sf::Keyboard::Key::Escape, sf::Keyboard::Scan::Escape);

  ASSERT_FALSE(connection().calls().empty());
  EXPECT_EQ(connection().calls().back().request,
            ServerConnectionCall::Request::Cancel);
  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::Title}));
}

/**
 * Given the waiting room
 * When Escape is pressed and released
 * Then the server is left and the screen closed
 */
TEST_F(LobbyScreenFixture, EscapeLeavesTheServer) {
  tap(sf::Keyboard::Key::Escape, sf::Keyboard::Scan::Escape);

  ASSERT_FALSE(connection().calls().empty());
  EXPECT_EQ(connection().calls().back().request,
            ServerConnectionCall::Request::Cancel);
  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the waiting room
 * When the BACK chip is clicked, and the press shows
 * Then the server is left and the screen closed
 */
TEST_F(LobbyScreenFixture, TheBackChipLeavesTheServer) {
  screen().handleEvent(leftPress(BACK_CHIP_MIDDLE));
  screen().handleEvent(leftRelease(BACK_CHIP_MIDDLE));
  screen().update(PRESS_DURATION);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the waiting room, READY selected
 * When Enter activates READY, and the press lasts
 * Then the click of READY plays once
 */
TEST_F(LobbyScreenFixture, ReadyPlaysTheClickOnce) {
  tap(sf::Keyboard::Key::Enter, sf::Keyboard::Scan::Enter);

  screen().update(PRESS_DURATION);

  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}

/**
 * Given the waiting room
 * When the ready key is pressed and released
 * Then the click of READY plays, as when READY is pressed
 */
TEST_F(LobbyScreenFixture, TheReadyKeyPlaysTheClickOfReady) {
  tap(sf::Keyboard::Key::R, READY_KEY);

  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}

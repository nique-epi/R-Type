#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "BackgroundColors.hpp"
#include "ConnectionStatus.hpp"
#include "KeyBindings.hpp"
#include "KeyPress.hpp"
#include "LobbyScreen.hpp"
#include "LobbyState.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "ScreenId.hpp"
#include "ScreenTestConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::ConnectionStatus;
using rtype::client::KeyBindings;
using rtype::client::LobbyScreen;
using rtype::client::MotionPreference;
using rtype::client::PRESS_DURATION;
using rtype::client::ScreenId;
using rtype::client::STAR_GOLD;
using rtype::network::LobbyPlayer;
using ::testing::Contains;
using ::testing::ElementsAre;

namespace {

/** @brief The local player, and another one in the third bay. */
constexpr std::uint8_t OWN_NUMBER = 1;
constexpr std::uint8_t OTHER_NUMBER = 3;
constexpr const char* OWN_NICKNAME = "ACE";
constexpr const char* OTHER_NICKNAME = "CAPTAIN_STARDUST";

/**
 * @brief Where the waiting room the team chose writes things: the nicknames
 * of the first and third bays on their middle columns, row 120; YOU at the
 * end of the number row of the first bay; the inside of the cell of the
 * first bay, 8 in and 86 down less its border; the middle of the BACK chip, in
 * pixels of the window.
 */
constexpr sf::Vector2i FIRST_BAY_NICKNAME(75, 120);
constexpr sf::Vector2i THIRD_BAY_NICKNAME(295, 120);
constexpr sf::Vector2i FIRST_BAY_LOCAL_TAG(118, 58);
constexpr sf::IntRect FIRST_BAY_CELL_INSIDE({33, 137}, {84, 14});
constexpr sf::Vector2i BACK_CHIP_MIDDLE =
    sf::Vector2i(415, 238) * INTERFACE_PIXEL;

/** @brief The name the keycap of READY shows in these tests. */
std::string readyKeyName([[maybe_unused]] sf::Keyboard::Scancode key) {
  return "R";
}

LobbyPlayer pilot(std::uint8_t number, const char* nickname, bool ready) {
  return {.playerNumber = number, .ready = ready, .nickname = nickname};
}

const ServerConnectionCall ASK_READY{
    .request = ServerConnectionCall::Request::SetReady,
    .server = {},
    .nickname = {},
    .ready = true};
const ServerConnectionCall ASK_NOT_READY{
    .request = ServerConnectionCall::Request::SetReady,
    .server = {},
    .nickname = {},
    .ready = false};

/** @brief A waiting room alone, the local player accepted as player 1, with
 * a recording navigator, connection and canvas, and the default keys. */
class LobbyScreenTest : public ::testing::Test {
 protected:
  LobbyScreenTest() {
    connection_.setStatus({.phase = ConnectionStatus::Phase::Accepted,
                           .playerNumber = OWN_NUMBER});
  }

  /** @brief The server lists these players, and a frame goes by. */
  void list(std::vector<LobbyPlayer> players) {
    connection_.setLobbyPlayers(std::move(players));
    screen_.update(ONE_FRAME);
  }

  /** @brief Presses then releases a key, by its place on the keyboard. */
  void tap(sf::Keyboard::Key key, sf::Keyboard::Scancode scancode) {
    screen_.handleEvent(keyPress(key, scancode));
    screen_.handleEvent(keyRelease(key, scancode));
  }

  /** @returns The ready states asked of the connection, in order. */
  [[nodiscard]] std::vector<ServerConnectionCall> readyRequests() const {
    std::vector<ServerConnectionCall> requests;
    std::ranges::copy_if(connection_.calls(), std::back_inserter(requests),
                         [](const ServerConnectionCall& call) noexcept {
                           return call.request ==
                                  ServerConnectionCall::Request::SetReady;
                         });
    return requests;
  }

  [[nodiscard]] const RecordingNavigator& navigator() const {
    return navigator_;
  }
  [[nodiscard]] RecordingServerConnection& connection() { return connection_; }
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const { return canvas_; }
  [[nodiscard]] LobbyScreen& screen() { return screen_; }

 private:
  RecordingNavigator navigator_;
  RecordingServerConnection connection_;
  InterfaceCanvasRecord canvas_;
  LobbyScreen screen_{navigator_,
                      connection_,
                      std::make_unique<RecordingInterfaceCanvas>(canvas_),
                      samePlayfieldPoint,
                      KeyBindings{},
                      readyKeyName,
                      MotionPreference::Full};
};

}  // namespace

/**
 * Given the server lists players 1 and 3
 * When a frame goes by
 * Then each nickname is written in the bay of its number
 */
TEST_F(LobbyScreenTest, EachPilotTakesTheBayOfItsNumber) {
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
TEST_F(LobbyScreenTest, APlaceNobodyTookIsAnOpenBay) {
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
TEST_F(LobbyScreenTest, TheLocalPlayerIsTaggedYou) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  EXPECT_EQ(anchorOf(canvas(), "YOU"), FIRST_BAY_LOCAL_TAG);
}

/**
 * Given the server lists two players
 * When a frame goes by
 * Then the top-right corner says 2 PILOTS
 */
TEST_F(LobbyScreenTest, ThePilotCountSaysHowManyAreListed) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false),
        pilot(OTHER_NUMBER, OTHER_NICKNAME, false)});

  EXPECT_NE(writingOf(canvas(), "2 PILOTS"), nullptr);
}

/**
 * Given the server lists the local player alone
 * When a frame goes by
 * Then the count is written in the singular
 */
TEST_F(LobbyScreenTest, ASinglePilotIsCountedInTheSingular) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  EXPECT_NE(writingOf(canvas(), "1 PILOT"), nullptr);
}

/**
 * Given two listed pilots, neither ready
 * When a frame goes by
 * Then the status says 2 PILOTS NOT READY, naming no open place
 */
TEST_F(LobbyScreenTest, SeveralPilotsNotReadyAreCounted) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false),
        pilot(OTHER_NUMBER, OTHER_NICKNAME, false)});

  EXPECT_NE(writingOf(canvas(), "2 PILOTS NOT READY"), nullptr);
}

/**
 * Given the local player ready and another pilot not ready
 * When a frame goes by
 * Then the status names the other pilot
 */
TEST_F(LobbyScreenTest, ASinglePilotNotReadyIsNamed) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, true),
        pilot(OTHER_NUMBER, OTHER_NICKNAME, false)});

  EXPECT_NE(writingOf(canvas(), "CAPTAIN_STARDUST IS NOT READY"), nullptr);
}

/**
 * Given another pilot ready and the local player not ready
 * When a frame goes by
 * Then the status says YOU ARE NOT READY
 */
TEST_F(LobbyScreenTest, TheLocalPlayerAloneNotReadyIsToldSo) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false),
        pilot(OTHER_NUMBER, OTHER_NICKNAME, true)});

  EXPECT_NE(writingOf(canvas(), "YOU ARE NOT READY"), nullptr);
}

/**
 * Given two listed pilots, both ready, two places open
 * When a frame goes by
 * Then the status says the game is launching: open places are not waited for
 */
TEST_F(LobbyScreenTest, EveryListedPilotReadyLaunches) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, true),
        pilot(OTHER_NUMBER, OTHER_NICKNAME, true)});

  EXPECT_NE(writingOf(canvas(), "ALL READY: LAUNCHING"), nullptr);
}

/**
 * Given the local player not ready
 * When the R key is pressed and released
 * Then the connection is asked to set the player ready
 */
TEST_F(LobbyScreenTest, TheReadyKeyAsksToBeReady) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  tap(sf::Keyboard::Key::R, sf::Keyboard::Scan::R);

  EXPECT_THAT(readyRequests(), ElementsAre(ASK_READY));
}

/**
 * Given the local player asked to be ready
 * When the R key is pressed and released
 * Then the connection is asked to set the player not ready
 */
TEST_F(LobbyScreenTest, TheReadyKeyAsksNotToBeReadyOnceAsked) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});
  connection().setRequestedReady(true);

  tap(sf::Keyboard::Key::R, sf::Keyboard::Scan::R);

  EXPECT_THAT(readyRequests(), ElementsAre(ASK_NOT_READY));
}

/**
 * Given the local player not ready
 * When the R key is held, the keyboard repeating its press
 * Then the connection is asked once
 */
TEST_F(LobbyScreenTest, HoldingTheReadyKeyAsksOnce) {
  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  screen().handleEvent(keyPress(sf::Keyboard::Key::R, sf::Keyboard::Scan::R));
  screen().handleEvent(keyPress(sf::Keyboard::Key::R, sf::Keyboard::Scan::R));

  EXPECT_THAT(readyRequests(), ElementsAre(ASK_READY));
}

/**
 * Given READY selected
 * When Enter is pressed and released, and the press shows
 * Then the connection is asked to set the player ready
 */
TEST_F(LobbyScreenTest, TheReadyBarAsksToBeReady) {
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
TEST_F(LobbyScreenTest, TheBarSaysCancelReadyOnceAsked) {
  connection().setRequestedReady(true);

  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  EXPECT_NE(writingOf(canvas(), "CANCEL READY"), nullptr);
}

/**
 * Given the local player asked to be ready, the server still listing it not
 * ready
 * When a frame goes by
 * Then its cell says READY on gold
 */
TEST_F(LobbyScreenTest, AnAskTheServerDoesNotShowYetIsWaitedFor) {
  connection().setRequestedReady(true);

  list({pilot(OWN_NUMBER, OWN_NICKNAME, false)});

  EXPECT_NE(writingOf(canvas(), "READY"), nullptr);
  EXPECT_THAT(canvas().fills,
              Contains(RecordedFill{.area = FIRST_BAY_CELL_INSIDE,
                                    .color = STAR_GOLD}));
}

/**
 * Given the connection learned that the game started
 * When two frames go by
 * Then the game is asked for in place of every screen, once
 */
TEST_F(LobbyScreenTest, TheGameIsAskedOnceItStarted) {
  connection().setGameStarted(true);

  screen().update(ONE_FRAME);
  screen().update(ONE_FRAME);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::Game}));
}

/**
 * Given the waiting room
 * When Escape is pressed and released
 * Then the server is left and the screen closed
 */
TEST_F(LobbyScreenTest, EscapeLeavesTheServer) {
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
TEST_F(LobbyScreenTest, TheBackChipLeavesTheServer) {
  screen().handleEvent(leftPress(BACK_CHIP_MIDDLE));
  screen().handleEvent(leftRelease(BACK_CHIP_MIDDLE));
  screen().update(PRESS_DURATION);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

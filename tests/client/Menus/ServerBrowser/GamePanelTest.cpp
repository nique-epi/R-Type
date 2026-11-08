#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "GameListSnapshot.hpp"
#include "GamePanel.hpp"
#include "GameRow.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ServerListPhase.hpp"
#include "ServerMapScreenFixtures.hpp"
#include "ServerMapScreenTestConstants.hpp"

using rtype::client::GameListSnapshot;
using rtype::client::GamePanel;
using rtype::client::GameRow;
using rtype::client::ServerListPhase;
using rtype::client::testing::LYON_SERVER_NAME;
using rtype::client::testing::lyonGames;
using rtype::client::testing::NEW_GAME_LINE;
using rtype::client::testing::PLAYING_GAME_ROW;
using rtype::client::testing::SEVEN_GAMES;
using rtype::client::testing::SEVEN_ROWS_DOWN;
using rtype::client::testing::SEVENTH_GAME_NUMBER;
using rtype::client::testing::WAITING_GAME_LINE;
using rtype::client::testing::WAITING_GAME_NUMBER;
using rtype::client::testing::waitingGames;

/**
 * Given the games of Lyon
 * When the panel shows them
 * Then + NEW GAME is chosen, and no game can be joined from it
 */
TEST(GamePanel, OpensOnNewGame) {
  GamePanel panel;
  panel.show(lyonGames(), std::string{LYON_SERVER_NAME});

  EXPECT_TRUE(panel.newGameChosen());
  EXPECT_FALSE(panel.joinable().has_value());
}

/**
 * Given the games of Lyon, + NEW GAME chosen
 * When the choice moves one row down
 * Then the waiting game is chosen and can be joined
 */
TEST(GamePanel, TheWaitingGameStandsUnderNewGame) {
  GamePanel panel;
  panel.show(lyonGames(), std::string{LYON_SERVER_NAME});

  panel.move(1);

  EXPECT_EQ(panel.joinable().value_or(GameRow{}).number, WAITING_GAME_NUMBER);
}

/**
 * Given the games of Lyon
 * When the choice moves down to the game in progress
 * Then nothing can be joined
 */
TEST(GamePanel, AGameInProgressCannotBeJoined) {
  GamePanel panel;
  panel.show(lyonGames(), std::string{LYON_SERVER_NAME});

  panel.move(PLAYING_GAME_ROW);

  EXPECT_FALSE(panel.joinable().has_value());
  EXPECT_FALSE(panel.newGameChosen());
}

/**
 * Given the games of Lyon, the game in progress chosen
 * When the list is shown again, read anew
 * Then the game in progress stays chosen, by its number, and the choice does
 * not fall back to + NEW GAME
 */
TEST(GamePanel, ANewListKeepsTheChosenGame) {
  GamePanel panel;
  panel.show(lyonGames(), std::string{LYON_SERVER_NAME});
  panel.move(PLAYING_GAME_ROW);

  panel.show(lyonGames(), std::string{LYON_SERVER_NAME});

  EXPECT_FALSE(panel.newGameChosen());
  EXPECT_FALSE(panel.joinable().has_value());
}

/**
 * Given seven waiting games under + NEW GAME
 * When the choice moves seven rows down, past the six rows shown
 * Then the seventh game is chosen, and painted on the last row shown
 */
TEST(GamePanel, ScrollsToKeepTheChosenGameShown) {
  GamePanel panel;
  panel.show(waitingGames(SEVEN_GAMES), std::string{LYON_SERVER_NAME});

  panel.move(SEVEN_ROWS_DOWN);

  EXPECT_EQ(panel.joinable().value_or(GameRow{}).number, SEVENTH_GAME_NUMBER);
}

/**
 * Given the games of Lyon, + NEW GAME chosen
 * When the choice moves up, above the first row
 * Then + NEW GAME stays chosen
 */
TEST(GamePanel, StopsAtNewGame) {
  GamePanel panel;
  panel.show(lyonGames(), std::string{LYON_SERVER_NAME});

  panel.move(-1);

  EXPECT_TRUE(panel.newGameChosen());
}

/**
 * Given no game at all
 * When the choice moves
 * Then nothing can be joined, and nothing is read out of the list
 */
TEST(GamePanel, AnEmptyListHasNothingToJoin) {
  GamePanel panel;
  panel.show(waitingGames(0), std::string{LYON_SERVER_NAME});

  panel.move(1);

  EXPECT_FALSE(panel.joinable().has_value());
}

/**
 * Given a server the master listed with no game
 * When the panel shows its games
 * Then + NEW GAME is offered: a game can be opened on an empty server
 */
TEST(GamePanel, AServerWithNoGameOffersNewGame) {
  GamePanel panel;
  panel.show(GameListSnapshot{.phase = ServerListPhase::Empty,
                              .games = {},
                              .difficulties = {},
                              .server = {}},
             std::string{LYON_SERVER_NAME});

  EXPECT_TRUE(panel.newGameChosen());
}

/**
 * Given games the master has not answered yet
 * When the panel shows them
 * Then + NEW GAME is not offered, since the difficulties are not known yet
 */
TEST(GamePanel, ASearchingListOffersNoNewGame) {
  GamePanel panel;
  panel.show(GameListSnapshot{.phase = ServerListPhase::Searching,
                              .games = {},
                              .difficulties = {},
                              .server = {}},
             std::string{LYON_SERVER_NAME});

  EXPECT_FALSE(panel.newGameChosen());
}

/**
 * Given the games of Lyon, + NEW GAME chosen
 * When the panel is painted
 * Then the line under the rows says that ENTER sets the rules of a new game
 */
TEST(GamePanel, NewGameSaysEnterSetsItsRules) {
  GamePanel panel;
  panel.show(lyonGames(), std::string{LYON_SERVER_NAME});
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  panel.paint(canvas);

  EXPECT_NE(writingOf(record, std::string{NEW_GAME_LINE}), nullptr);
}

/**
 * Given the games of Lyon, the waiting game chosen, played by the default
 * rules
 * When the panel is painted
 * Then the line under the rows names the game with its rules before ENTER
 */
TEST(GamePanel, AWaitingGameNamesItsRules) {
  GamePanel panel;
  panel.show(lyonGames(), std::string{LYON_SERVER_NAME});
  panel.move(1);
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  panel.paint(canvas);

  EXPECT_NE(writingOf(record, std::string{WAITING_GAME_LINE}), nullptr);
}

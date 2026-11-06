#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "GamePanel.hpp"
#include "GameRow.hpp"
#include "ServerMapScreenFixtures.hpp"
#include "ServerMapScreenTestConstants.hpp"

using rtype::client::GamePanel;
using rtype::client::GameRow;
using rtype::client::testing::LYON_SERVER_NAME;
using rtype::client::testing::lyonGames;
using rtype::client::testing::SEVEN_GAMES;
using rtype::client::testing::SEVENTH_GAME_NUMBER;
using rtype::client::testing::SIX_ROWS_DOWN;
using rtype::client::testing::WAITING_GAME_NUMBER;
using rtype::client::testing::waitingGames;

/**
 * Given the games of Lyon, the waiting one first
 * When the panel shows them
 * Then the waiting game is chosen and can be joined
 */
TEST(GamePanel, ChoosesTheFirstGameAndCanJoinItWhenItWaits) {
  GamePanel panel;
  panel.show(lyonGames(), std::string{LYON_SERVER_NAME});

  ASSERT_TRUE(panel.joinable().has_value());
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

  panel.move(1);

  EXPECT_FALSE(panel.joinable().has_value());
}

/**
 * Given the games of Lyon, the game in progress chosen
 * When the list is shown again, read anew
 * Then the game in progress stays chosen, by its number
 */
TEST(GamePanel, ANewListKeepsTheChosenGame) {
  GamePanel panel;
  panel.show(lyonGames(), std::string{LYON_SERVER_NAME});
  panel.move(1);

  panel.show(lyonGames(), std::string{LYON_SERVER_NAME});

  EXPECT_FALSE(panel.joinable().has_value());
}

/**
 * Given seven waiting games
 * When the choice moves six rows down, past the six rows shown
 * Then the seventh game is chosen, and painted on the last row shown
 */
TEST(GamePanel, ScrollsToKeepTheChosenGameShown) {
  GamePanel panel;
  panel.show(waitingGames(SEVEN_GAMES), std::string{LYON_SERVER_NAME});

  panel.move(SIX_ROWS_DOWN);

  EXPECT_EQ(panel.joinable().value_or(GameRow{}).number, SEVENTH_GAME_NUMBER);
}

/**
 * Given the games of Lyon, the first one chosen
 * When the choice moves up, above the first game
 * Then the first game stays chosen
 */
TEST(GamePanel, StopsAtTheFirstGame) {
  GamePanel panel;
  panel.show(lyonGames(), std::string{LYON_SERVER_NAME});

  panel.move(-1);

  EXPECT_EQ(panel.joinable().value_or(GameRow{}).number, WAITING_GAME_NUMBER);
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <string>
#include "ConnectionRequest.hpp"
#include "GameRequest.hpp"
#include "GameRules.hpp"
#include "GameRulesTestConstants.hpp"
#include "MenuScreenIdConstants.hpp"
#include "NewGameDialogFixture.hpp"
#include "NewGameDialogTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"

using rtype::client::CONNECTION_TO_CHOSEN_SERVER_SCREEN;
using rtype::client::GameRequest;
using rtype::client::testing::EASY_SHOWN;
using rtype::client::testing::LIVES_AT_THE_FEWEST;
using rtype::client::testing::LIVES_AT_THE_MOST;
using rtype::client::testing::NORMAL_SHOWN;
using rtype::client::testing::PAST_THE_BOUNDS;
using rtype::client::testing::ROWS_DOWN_TO_CREATE;
using rtype::client::testing::ROWS_FROM_LIVES_TO_CREATE;
using rtype::client::testing::UNTOUCHED_LIVES;
using rtype::network::GameRules;
using rtype::network::NEW_GAME_CHOICE;
using rtype::testing::CHOSEN_DIFFICULTY;
using rtype::testing::CHOSEN_LIVES;
using rtype::testing::EASY_DIFFICULTY;
using rtype::testing::NORMAL_DIFFICULTY;
using rtype::testing::VETERAN_DIFFICULTY;
using ::testing::ElementsAre;

/**
 * Given a server that lists easy, normal and hard
 * When its new game dialog opens
 * Then the difficulty shown is NORMAL, the default one
 */
TEST_F(NewGameDialogFixture, OpensOnTheDefaultDifficulty) {
  open({std::string(EASY_DIFFICULTY), std::string(NORMAL_DIFFICULTY),
        std::string(CHOSEN_DIFFICULTY)});

  EXPECT_NE(writingOf(painting(), std::string(NORMAL_SHOWN)), nullptr);
}

/**
 * Given a server that listed no difficulty
 * When its new game dialog opens
 * Then it offers the default difficulty, NORMAL
 */
TEST_F(NewGameDialogFixture, AServerWithoutDifficultiesOffersTheDefault) {
  open({});

  EXPECT_NE(writingOf(painting(), std::string(NORMAL_SHOWN)), nullptr);
}

/**
 * Given a server whose tables are easy and veteran, without normal
 * When its new game dialog opens
 * Then the difficulty shown is its first one, EASY
 */
TEST_F(NewGameDialogFixture, AServerWithoutTheDefaultOpensOnItsFirst) {
  open({std::string(EASY_DIFFICULTY), std::string(VETERAN_DIFFICULTY)});

  EXPECT_NE(writingOf(painting(), std::string(EASY_SHOWN)), nullptr);
}

/**
 * Given the dialog of a server that lists easy, normal and hard
 * When the player steps the difficulty to hard, the lives to five and the
 * friendly fire on, then presses CREATE
 * Then the choice asks for a new game played by those rules
 */
TEST_F(NewGameDialogFixture, CreateAsksForANewGameWithTheChosenRules) {
  open({std::string(EASY_DIFFICULTY), std::string(NORMAL_DIFFICULTY),
        std::string(CHOSEN_DIFFICULTY)});
  choose(sf::Keyboard::Key::Right);
  choose(sf::Keyboard::Key::Down);
  choose(sf::Keyboard::Key::Right);
  choose(sf::Keyboard::Key::Right);
  choose(sf::Keyboard::Key::Down);
  choose(sf::Keyboard::Key::Right);
  choose(sf::Keyboard::Key::Down);
  choose(sf::Keyboard::Key::Down);

  choose(sf::Keyboard::Key::Enter);

  EXPECT_EQ(choice().lastRequest,
            (GameRequest{
                .game = NEW_GAME_CHOICE,
                .rules = GameRules{.difficulty = std::string(CHOSEN_DIFFICULTY),
                                   .lives = CHOSEN_LIVES,
                                   .friendlyFire = true}}));
}

/**
 * Given the dialog, CREATE chosen
 * When CREATE is pressed
 * Then the connection to the chosen server opens over the dialog
 */
TEST_F(NewGameDialogFixture, CreateOpensTheConnectionOverTheDialog) {
  open({std::string(NORMAL_DIFFICULTY)});
  for (int row = 0; row < ROWS_DOWN_TO_CREATE; ++row) {
    choose(sf::Keyboard::Key::Down);
  }

  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(
      navigator().calls(),
      ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                .screen = CONNECTION_TO_CHOSEN_SERVER_SCREEN}));
}

/**
 * Given the dialog, the lives row chosen
 * When Left is pressed more times than there are lives, then CREATE
 * Then the new game gives one life: the lives stop at the fewest a server
 * accepts
 */
TEST_F(NewGameDialogFixture, TheLivesStopAtOne) {
  open({std::string(NORMAL_DIFFICULTY)});
  choose(sf::Keyboard::Key::Down);
  for (int press = 0; press < PAST_THE_BOUNDS; ++press) {
    choose(sf::Keyboard::Key::Left);
  }
  for (int row = 0; row < ROWS_FROM_LIVES_TO_CREATE; ++row) {
    choose(sf::Keyboard::Key::Down);
  }

  choose(sf::Keyboard::Key::Enter);

  EXPECT_EQ(createdLives(), LIVES_AT_THE_FEWEST);
}

/**
 * Given the dialog, the lives row chosen
 * When Right is pressed more times than there are lives, then CREATE
 * Then the new game gives nine lives: the most a server accepts
 */
TEST_F(NewGameDialogFixture, TheLivesStopAtNine) {
  open({std::string(NORMAL_DIFFICULTY)});
  choose(sf::Keyboard::Key::Down);
  for (int press = 0; press < PAST_THE_BOUNDS; ++press) {
    choose(sf::Keyboard::Key::Right);
  }
  for (int row = 0; row < ROWS_FROM_LIVES_TO_CREATE; ++row) {
    choose(sf::Keyboard::Key::Down);
  }

  choose(sf::Keyboard::Key::Enter);

  EXPECT_EQ(createdLives(), LIVES_AT_THE_MOST);
}

/**
 * Given the dialog, untouched
 * When CREATE is pressed
 * Then the new game is played by the default lives, three
 */
TEST_F(NewGameDialogFixture, AnUntouchedDialogGivesThreeLives) {
  open({std::string(NORMAL_DIFFICULTY)});
  for (int row = 0; row < ROWS_DOWN_TO_CREATE; ++row) {
    choose(sf::Keyboard::Key::Down);
  }

  choose(sf::Keyboard::Key::Enter);

  EXPECT_EQ(createdLives(), UNTOUCHED_LIVES);
}

/**
 * Given the dialog, a value stepped
 * When Escape is pressed and released
 * Then the dialog closes and asks for no game
 */
TEST_F(NewGameDialogFixture, EscapeClosesWithoutAsking) {
  open({std::string(NORMAL_DIFFICULTY)});
  choose(sf::Keyboard::Key::Right);

  choose(sf::Keyboard::Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
  EXPECT_FALSE(choice().lastRequest.has_value());
}

/**
 * Given the dialog of the server the map listed
 * When CREATE is pressed
 * Then that server goes in the choice with the new game, as one attempt
 */
TEST_F(NewGameDialogFixture, CreateAsksTheServerOfTheList) {
  open({std::string(NORMAL_DIFFICULTY)});
  for (int row = 0; row < ROWS_DOWN_TO_CREATE; ++row) {
    choose(sf::Keyboard::Key::Down);
  }

  choose(sf::Keyboard::Key::Enter);

  EXPECT_EQ(choice().lastServer, std::optional{listedServer()});
}

/**
 * Given the dialog of the server the map listed
 * When Escape closes it
 * Then the choice keeps the server of its last attempt, none here
 */
TEST_F(NewGameDialogFixture, EscapeLeavesTheServerOfTheLastAttempt) {
  open({std::string(NORMAL_DIFFICULTY)});

  choose(sf::Keyboard::Key::Escape);

  EXPECT_FALSE(choice().lastServer.has_value());
}

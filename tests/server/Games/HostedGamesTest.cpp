#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "GameState.hpp"
#include "Games.hpp"
#include "HostedGame.hpp"
#include "HostedGameState.hpp"
#include "HostedGames.hpp"
#include "HostedGamesFixture.hpp"
#include "HostedGamesTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "ServerModuleTestConstants.hpp"

using rtype::network::HostedGame;
using rtype::network::HostedGameState;
using rtype::network::ServerHeartbeat;
using rtype::server::GameState;
using rtype::server::hostedStateOf;

/**
 * Given three games of the server, none with a player
 * When the master lists them
 * Then they are GAME 1 to GAME 3, numbered from 1, all waiting, with the
 * difficulty they are played in and no player
 */
TEST_F(HostedGamesFixture, NamesAndNumbersTheGamesFromOne) {
  for (std::size_t game = 0; game < THREE_GAMES; ++game) {
    addGame();
  }

  const std::vector<HostedGame> hosted = games().hostedGames();

  ASSERT_EQ(hosted.size(), THREE_GAMES);
  EXPECT_EQ(hosted.at(0).name, FIRST_HOSTED_NAME);
  EXPECT_EQ(hosted.at(1).name, SECOND_HOSTED_NAME);
  EXPECT_EQ(hosted.at(2).name, THIRD_HOSTED_NAME);
  EXPECT_EQ(hosted.at(0).number, 1U);
  EXPECT_EQ(hosted.at(2).number, THREE_GAMES);
  EXPECT_EQ(hosted.at(1).difficulty, HOSTED_DIFFICULTY);
  EXPECT_EQ(hosted.at(1).state, HostedGameState::Waiting);
  EXPECT_TRUE(hosted.at(1).players.empty());
}

/**
 * Given a game whose players sat in the order ACE, BYDO_HUNTER, R9
 * When the master lists it
 * Then it lists their nicknames in that order, and the other game none
 */
TEST_F(HostedGamesFixture, ListsThePlayersOfEachGameInSeatOrder) {
  addGame();
  addGame();
  seat(*games().all().front(), 0, std::string{FIRST_HOSTED_PILOT});
  seat(*games().all().front(), 1, std::string{SECOND_HOSTED_PILOT});
  seat(*games().all().front(), 2, std::string{THIRD_HOSTED_PILOT});

  const std::vector<HostedGame> hosted = games().hostedGames();

  EXPECT_EQ(hosted.at(0).players,
            (std::vector<std::string>{std::string{FIRST_HOSTED_PILOT},
                                      std::string{SECOND_HOSTED_PILOT},
                                      std::string{THIRD_HOSTED_PILOT}}));
  EXPECT_TRUE(hosted.at(1).players.empty());
}

/**
 * Given a game that waits with every seat taken
 * When the master lists it
 * Then it is full
 */
TEST_F(HostedGamesFixture, ALobbyWithEverySeatTakenIsFull) {
  addGame();
  fill(*games().all().front());

  EXPECT_EQ(games().hostedGames().front().state, HostedGameState::Full);
}

/**
 * Given a game that waits with one seat free
 * When the master lists it
 * Then it is waiting
 */
TEST_F(HostedGamesFixture, ALobbyWithASeatFreeIsWaiting) {
  addGame();
  seat(*games().all().front(), 0, std::string{FIRST_HOSTED_PILOT});
  seat(*games().all().front(), 1, std::string{SECOND_HOSTED_PILOT});
  seat(*games().all().front(), 2, std::string{THIRD_HOSTED_PILOT});

  EXPECT_EQ(games().hostedGames().front().state, HostedGameState::Waiting);
}

/**
 * Given the games of the server before and after a player takes a seat
 * When the master lists them each time
 * Then the two lists differ, and a list asked twice is the same
 */
TEST_F(HostedGamesFixture, AJoiningPlayerChangesTheList) {
  addGame();
  const std::vector<HostedGame> before = games().hostedGames();

  EXPECT_EQ(games().hostedGames(), before);
  seat(*games().all().front(), 0, std::string{FIRST_HOSTED_PILOT});

  EXPECT_NE(games().hostedGames(), before);
}

/**
 * Given games with players
 * When their listing is written as a heartbeat and read back by the master
 * Then the master reads the same games
 */
TEST_F(HostedGamesFixture, TheMasterReadsWhatTheServerWrites) {
  addGame();
  addGame();
  seat(*games().all().front(), 0, std::string{FIRST_HOSTED_PILOT});
  fill(*games().all().back());
  const ServerHeartbeat heartbeat{.games = games().hostedGames(),
                                  .measures = {}};

  EXPECT_EQ(rtype::network::heartbeatFromText(
                rtype::network::heartbeatText(heartbeat))
                .games,
            heartbeat.games);
}

/**
 * Given a game that waits, starts, plays or is over
 * When the master is told its state with four players, then with none
 * Then only a game that waits is full, and the other states keep their name
 */
TEST(HostedStates, OnlyAWaitingGameIsEverFull) {
  EXPECT_EQ(hostedStateOf(GameState::WaitingForPlayers, FOUR_PLAYERS),
            HostedGameState::Full);
  EXPECT_EQ(hostedStateOf(GameState::WaitingForPlayers, THREE_PLAYERS),
            HostedGameState::Waiting);
  EXPECT_EQ(hostedStateOf(GameState::WaitingForPlayers, NO_PLAYER),
            HostedGameState::Waiting);
  EXPECT_EQ(hostedStateOf(GameState::Starting, FOUR_PLAYERS),
            HostedGameState::Starting);
  EXPECT_EQ(hostedStateOf(GameState::InProgress, FOUR_PLAYERS),
            HostedGameState::Playing);
  EXPECT_EQ(hostedStateOf(GameState::Finished, FOUR_PLAYERS),
            HostedGameState::Finished);
  EXPECT_EQ(hostedStateOf(GameState::Finished, NO_PLAYER),
            HostedGameState::Finished);
}

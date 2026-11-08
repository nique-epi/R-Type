#include <gtest/gtest.h>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include "BanFixtures.hpp"
#include "BanKind.hpp"
#include "BanTestConstants.hpp"
#include "BansDatabase.hpp"
#include "ConnectionRedirect.hpp"
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "GameRules.hpp"
#include "GameStatus.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerSupervisedGamesFixture.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesReportTestConstants.hpp"
#include "SupervisedGamesTestConstants.hpp"

using rtype::network::ConnectionRedirect;
using rtype::network::ConnectionRefused;
using rtype::network::GameRules;
using rtype::network::GameStatusState;
using rtype::network::IncomingDatagram;
using rtype::network::NEW_GAME_CHOICE;
using rtype::network::RefusalReason;
using rtype::server::BanKind;
using rtype::server::BansDatabase;
using rtype::server::DIFFICULTY_OPTION;
using rtype::server::FRIENDLY_FIRE_OPTION;
using rtype::server::LIVES_OPTION;
using rtype::server::testing::banOf;
using rtype::server::testing::LONG_AGO;
using rtype::server::testing::onlyMessageIn;
using rtype::server::testing::sentTo;

/**
 * Given a supervisor with no game
 * When a stranger asks for a place without a cookie
 * Then it gets a challenge and no game process starts
 */
TEST_F(ServerSupervisedGamesFixture, ChallengesAStrangerAndStartsNothing) {
  startSupervisor();

  runTick({}, {IncomingDatagram{.sender = supervisedPlayer(0),
                                .payload = requestDatagram()}});

  EXPECT_EQ(sentTo(network().sender().sent(), supervisedPlayer(0)).size(), 1U);
  EXPECT_TRUE(launcher().commands().empty());
}

/**
 * Given a game process that started and said nothing yet
 * When the same player asks again with its cookie
 * Then it gets no redirect yet and no second game starts
 */
TEST_F(ServerSupervisedGamesFixture, AnswersNothingWhileTheGameStarts) {
  startSupervisor();
  askForAPlaceAs(0);

  askForAPlaceAs(0);

  EXPECT_EQ(launcher().commands().size(), 1U);
  EXPECT_EQ(sentTo(network().sender().sent(), supervisedPlayer(0)).size(), 2U);
}

/**
 * Given four players who ask for a place one after the other
 * When the supervisor reads their requests
 * Then it starts a single game
 */
TEST_F(ServerSupervisedGamesFixture, StartsOnlyOneGameForSeveralRequests) {
  startSupervisor();
  for (std::size_t player = 0; player < 4; ++player) {
    askForAPlaceAs(player);
  }

  EXPECT_EQ(launcher().commands().size(), 1U);
}

/**
 * Given a game that reported it waits for players
 * When a player asks again with its cookie
 * Then it gets a redirect to the port of that game
 */
TEST_F(ServerSupervisedGamesFixture, RedirectsAPlayerOnceTheGameIsReady) {
  startSupervisor();
  askForAPlaceAs(0);
  gameReports(0, statusWithPlayers(GameStatusState::WaitingForPlayers, 0),
              SEQUENCE_FIVE);

  askForAPlaceAs(0);

  const auto answers = sentTo(network().sender().sent(), supervisedPlayer(0));
  EXPECT_EQ(
      std::to_string(onlyMessageIn<ConnectionRedirect>(answers.back()).port),
      argumentAfter(launcher().commands().front(), "-p"));
}

/**
 * Given a game whose status "in progress, four players" of sequence 5 was read
 * When an older status "waiting, no player" of sequence 4 arrives, then a new
 * player asks for a place
 * Then a second game starts: the older status did not undo the newer one
 */
TEST_F(ServerSupervisedGamesFixture, AnOlderStatusDoesNotUndoANewerOne) {
  startSupervisor();
  askForAPlaceAs(0);
  gameReports(0, statusWithPlayers(GameStatusState::InProgress, 4),
              SEQUENCE_FIVE);
  gameReports(0, statusWithPlayers(GameStatusState::WaitingForPlayers, 0),
              SEQUENCE_FOUR);

  askForAPlaceAs(1);

  EXPECT_EQ(launcher().commands().size(), 2U);
}

/**
 * Given eight games in progress and full
 * When a ninth player asks for a place
 * Then it is refused as ServerFull and no ninth game starts
 */
TEST_F(ServerSupervisedGamesFixture, RefusesAPlayerWhenEveryGameIsFull) {
  startSupervisor();
  for (std::size_t game = 0; game < EIGHT_GAMES; ++game) {
    askForAPlaceAs(game);
    gameReports(game, statusWithPlayers(GameStatusState::InProgress, 4),
                SEQUENCE_FIVE);
  }

  askForAPlaceAs(EIGHT_GAMES);

  EXPECT_EQ(launcher().commands().size(), EIGHT_GAMES);
  const auto answers =
      sentTo(network().sender().sent(), supervisedPlayer(EIGHT_GAMES));
  EXPECT_EQ(onlyMessageIn<ConnectionRefused>(answers.back()).reason,
            RefusalReason::ServerFull);
}

/**
 * Given a request of another protocol version, long enough to be answered
 * When the supervisor reads it
 * Then it refuses it as IncompatibleVersion
 */
TEST_F(ServerSupervisedGamesFixture, RefusesARequestOfAnotherVersion) {
  startSupervisor();
  rtype::network::PacketHeader header;
  header.version = rtype::network::PROTOCOL_VERSION + 1;

  runTick({}, {IncomingDatagram{.sender = supervisedPlayer(0),
                                .payload = requestDatagram(header)}});

  const auto answers = sentTo(network().sender().sent(), supervisedPlayer(0));
  ASSERT_EQ(answers.size(), 1U);
  EXPECT_EQ(onlyMessageIn<ConnectionRefused>(answers.front()).reason,
            RefusalReason::IncompatibleVersion);
}

/**
 * Given a supervisor with no game
 * When five players ask for a place one after the other, before any status
 * Then a second game starts for the fifth: a game has four seats, and the
 * player whose request opened a game holds one of them
 */
TEST_F(ServerSupervisedGamesFixture, OpensASecondGameForTheFifthOfABurst) {
  startSupervisor();

  for (std::size_t player = 0; player <= FIFTH_PLAYER; ++player) {
    askForAPlaceAs(player);
  }

  EXPECT_EQ(launcher().commands().size(), 2U);
}

/**
 * Given a supervisor with no game
 * When a first player asks twice, then three other players ask
 * Then a single game starts: the player that asked twice holds one seat
 */
TEST_F(ServerSupervisedGamesFixture, APlayerThatAsksTwiceHoldsOneSeat) {
  startSupervisor();
  askForAPlaceAs(0);
  askForAPlaceAs(0);

  for (std::size_t player = 1; player < FOUR_PLAYERS; ++player) {
    askForAPlaceAs(player);
  }

  EXPECT_EQ(launcher().commands().size(), 1U);
}

/**
 * Given two games opened for the new games of two players, both waiting for
 * players
 * When a third player asks for the game in the second place
 * Then it gets a redirect to the port of the second game
 */
TEST_F(ServerSupervisedGamesFixture, RedirectsAPlayerToTheGameItNames) {
  startSupervisor();
  askForAPlaceAs(0, NEW_GAME_CHOICE);
  askForAPlaceAs(1, NEW_GAME_CHOICE);
  gameReports(0, statusWithPlayers(GameStatusState::WaitingForPlayers, 0),
              SEQUENCE_FIVE);
  gameReports(1, statusWithPlayers(GameStatusState::WaitingForPlayers, 0),
              SEQUENCE_FIVE);

  askForAPlaceAs(2, choiceOfGame(SECOND_PLACE));

  const auto answers = sentTo(network().sender().sent(), supervisedPlayer(2));
  EXPECT_EQ(
      std::to_string(onlyMessageIn<ConnectionRedirect>(answers.back()).port),
      argumentAfter(launcher().commands().at(1), "-p"));
}

/**
 * Given a supervisor with no game
 * When a player asks for a new game three times, as its client repeats it
 * Then a single game process starts
 */
TEST_F(ServerSupervisedGamesFixture, StartsOneGameForARepeatedNewGame) {
  startSupervisor();

  for (std::size_t request = 0; request < REPEATED_REQUESTS; ++request) {
    askForAPlaceAs(0, NEW_GAME_CHOICE);
  }

  EXPECT_EQ(launcher().commands().size(), 1U);
}

/**
 * Given a supervisor with no game
 * When a player asks for the game in the first place
 * Then it is refused as GameNotFound and no game process starts
 */
TEST_F(ServerSupervisedGamesFixture,
       RefusesAGameItDoesNotRunWithoutStartingOne) {
  startSupervisor();

  askForAPlaceAs(0, choiceOfGame(FIRST_PLACE));

  EXPECT_TRUE(launcher().commands().empty());
  const auto answers = sentTo(network().sender().sent(), supervisedPlayer(0));
  EXPECT_EQ(onlyMessageIn<ConnectionRefused>(answers.back()).reason,
            RefusalReason::GameNotFound);
}

/**
 * Given a supervisor that read the shipped waves file
 * When a player asks for a new game of the difficulty "nightmare", which the
 * file has no table for
 * Then it is refused as UnknownDifficulty and no game process starts
 */
TEST_F(ServerSupervisedGamesFixture, RefusesANewGameOfADifficultyItHasNot) {
  startSupervisor();
  GameRules rules = chosenRules();
  rules.difficulty = std::string{UNKNOWN_DIFFICULTY};

  askForAPlaceAs(0, NEW_GAME_CHOICE, rules);

  EXPECT_TRUE(launcher().commands().empty());
  const auto answers = sentTo(network().sender().sent(), supervisedPlayer(0));
  EXPECT_EQ(onlyMessageIn<ConnectionRefused>(answers.back()).reason,
            RefusalReason::UnknownDifficulty);
}

/**
 * Given a supervisor with no game
 * When a player asks for a new game of ten lives, one past the most
 * Then it is refused as LivesOutOfRange and no game process starts
 */
TEST_F(ServerSupervisedGamesFixture, RefusesANewGameOfTooManyLives) {
  startSupervisor();
  GameRules rules = chosenRules();
  rules.lives = LIVES_PAST_THE_MOST;

  askForAPlaceAs(0, NEW_GAME_CHOICE, rules);

  EXPECT_TRUE(launcher().commands().empty());
  const auto answers = sentTo(network().sender().sent(), supervisedPlayer(0));
  EXPECT_EQ(onlyMessageIn<ConnectionRefused>(answers.back()).reason,
            RefusalReason::LivesOutOfRange);
}

/**
 * Given a supervisor with no game
 * When a player asks for a new game of the difficulty "hard", five lives and
 * friendly fire
 * Then the game process starts with those rules
 */
TEST_F(ServerSupervisedGamesFixture, StartsANewGameWithTheRulesOfItsCreator) {
  startSupervisor();

  askForAPlaceAs(0, NEW_GAME_CHOICE, chosenRules());

  ASSERT_EQ(launcher().commands().size(), 1U);
  const auto& command = launcher().commands().front();
  EXPECT_EQ(argumentAfter(command, DIFFICULTY_OPTION), CHOSEN_DIFFICULTY);
  EXPECT_EQ(argumentAfter(command, LIVES_OPTION), CHOSEN_LIVES_ARGUMENT);
  EXPECT_EQ(argumentAfter(command, FRIENDLY_FIRE_OPTION),
            FRIENDLY_FIRE_ON_ARGUMENT);
}

/**
 * Given a supervisor with no game
 * When a player asks for any game
 * Then the game process starts with the default difficulty
 */
TEST_F(ServerSupervisedGamesFixture, StartsAnyGameWithTheDefaultRules) {
  startSupervisor();

  askForAPlaceAs(0);

  ASSERT_EQ(launcher().commands().size(), 1U);
  EXPECT_EQ(argumentAfter(launcher().commands().front(), DIFFICULTY_OPTION),
            SHIPPED_DEFAULT_DIFFICULTY);
}

/**
 * Given a ban for good of the address of the players, in the server file
 * before the supervisor starts
 * When a player of that address asks for a place, then sends its cookie back
 * Then it gets a challenge first, then ConnectionRefused Banned, and no game
 * starts
 */
TEST_F(ServerSupervisedGamesFixture, RefusesABannedPilotAfterItsCookie) {
  BansDatabase{gameConfigurationFile().parent_path() / SERVER_FILE_OF_PORT_4242}
      .save(banOf(BanKind::Address, SUPERVISED_PLAYER_ADDRESS, std::nullopt));
  startSupervisor();

  askForAPlaceAs(0);

  const auto answers = sentTo(network().sender().sent(), supervisedPlayer(0));
  EXPECT_EQ(onlyMessageIn<ConnectionRefused>(answers.back()).reason,
            RefusalReason::Banned);
  EXPECT_TRUE(launcher().commands().empty());
}

/**
 * Given a ban of the address of the players that ended long ago, in the
 * server file before the supervisor starts
 * When a player of that address asks for a place with its cookie
 * Then a game starts for it
 */
TEST_F(ServerSupervisedGamesFixture, StartsAGameForAPilotWhoseBanEnded) {
  BansDatabase{gameConfigurationFile().parent_path() / SERVER_FILE_OF_PORT_4242}
      .save(banOf(BanKind::Address, SUPERVISED_PLAYER_ADDRESS, LONG_AGO));
  startSupervisor();

  askForAPlaceAs(0);

  EXPECT_EQ(launcher().commands().size(), 1U);
}

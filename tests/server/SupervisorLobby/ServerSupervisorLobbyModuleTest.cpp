#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include "ConnectionRedirect.hpp"
#include "ConnectionRefused.hpp"
#include "GameStatus.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerDatagrams.hpp"
#include "ServerSupervisedGamesFixture.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesTestConstants.hpp"

using rtype::network::ConnectionRedirect;
using rtype::network::ConnectionRefused;
using rtype::network::GameStatus;
using rtype::network::GameStatusState;
using rtype::network::IncomingDatagram;
using rtype::network::RefusalReason;
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
  gameReports(
      0,
      GameStatus{.state = GameStatusState::WaitingForPlayers, .playerCount = 0},
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
  gameReports(
      0, GameStatus{.state = GameStatusState::InProgress, .playerCount = 4},
      SEQUENCE_FIVE);
  gameReports(
      0,
      GameStatus{.state = GameStatusState::WaitingForPlayers, .playerCount = 0},
      SEQUENCE_FOUR);

  askForAPlaceAs(1);

  EXPECT_EQ(launcher().commands().size(), 2U);
}

/**
 * Given eight games in progress and full
 * When a ninth player asks for a place
 * Then it is refused as GameFull and no ninth game starts
 */
TEST_F(ServerSupervisedGamesFixture, RefusesAPlayerWhenEveryGameIsFull) {
  startSupervisor();
  for (std::size_t game = 0; game < EIGHT_GAMES; ++game) {
    askForAPlaceAs(game);
    gameReports(
        game,
        GameStatus{.state = GameStatusState::InProgress, .playerCount = 4},
        SEQUENCE_FIVE);
  }

  askForAPlaceAs(EIGHT_GAMES);

  EXPECT_EQ(launcher().commands().size(), EIGHT_GAMES);
  const auto answers =
      sentTo(network().sender().sent(), supervisedPlayer(EIGHT_GAMES));
  EXPECT_EQ(onlyMessageIn<ConnectionRefused>(answers.back()).reason,
            RefusalReason::GameFull);
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

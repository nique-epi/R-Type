#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>
#include "ConnectionConstants.hpp"
#include "ConnectionStatus.hpp"
#include "DatagramCodec.hpp"
#include "EntityAnnouncements.hpp"
#include "GameStarted.hpp"
#include "LobbyState.hpp"
#include "PacketHeader.hpp"
#include "ReadyState.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionTestConstants.hpp"
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"
#include "WaitingRoomFixture.hpp"
#include "WorldStateFilterTestConstants.hpp"

namespace rtype::client {

using testing::ACCEPTED_PLAYER_NUMBER;
using testing::APPLIED_TICK;
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using testing::endOfOnePilot;
using testing::FIRST_SEQUENCE;
using testing::holds;
using testing::NICKNAME;
using testing::onlyMessageOf;
using testing::OTHER_NICKNAME;
using testing::OTHER_PLAYER_NUMBER;
using testing::OTHER_SESSION_TOKEN;
using testing::SECOND_SEQUENCE;
using testing::server;
using testing::SESSION_TOKEN;
using testing::stateListing;
using testing::WaitingRoomFixture;

using Phase = ConnectionStatus::Phase;

/**
 * Given an attempt the server just accepted as player 3
 * When the waiting room is read before any LobbyState
 * Then it holds player 3 alone, not ready, with the nickname it connected with
 */
TEST_F(WaitingRoomFixture, TheRoomHoldsThePlayerAloneOnceAccepted) {
  accept();

  EXPECT_THAT(
      waitingRoom().lobbyPlayers(),
      ElementsAre(network::LobbyPlayer{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                       .ready = false,
                                       .nickname = std::string{NICKNAME}}));
}

/**
 * Given a session the server opened with its token
 * When the player asks to be ready
 * Then a ReadyState saying ready goes at once, carrying the token
 */
TEST_F(WaitingRoomFixture, SendsReadyStateAtOnceWithTheSessionToken) {
  accept();

  waitingRoom().setReady(true);

  ASSERT_TRUE(holds<network::ReadyState>(sent().back()));
  EXPECT_EQ(onlyMessageOf<network::ReadyState>(sent().back()),
            network::ReadyState{.ready = true});
  EXPECT_EQ(
      network::DatagramCodec::decodeHeader(sent().back().payload).sessionToken,
      SESSION_TOKEN);
}

/**
 * Given an attempt the server has not answered
 * When the player asks to be ready
 * Then nothing is sent and nothing is asked
 */
TEST_F(WaitingRoomFixture, IgnoresReadyBeforeTheAcceptance) {
  connect();

  waitingRoom().setReady(true);

  EXPECT_EQ(sentCountOf<network::ReadyState>(), 0U);
  EXPECT_EQ(waitingRoom().requestedReady(), std::nullopt);
}

/**
 * Given a player who asked to be ready, whom the room still shows not ready
 * When one interval minus one nanosecond passes
 * Then the ReadyState is not sent again yet
 */
TEST_F(WaitingRoomFixture, WaitsOneIntervalBeforeSendingReadyStateAgain) {
  accept();
  waitingRoom().setReady(true);

  update(READY_STATE_INTERVAL - ONE_NANOSECOND);

  EXPECT_EQ(sentCountOf<network::ReadyState>(), 1U);
}

/**
 * Given a player who asked to be ready, whom the room still shows not ready
 * When one interval passes
 * Then the ReadyState is sent again, since UDP may have lost it
 */
TEST_F(WaitingRoomFixture, SendsReadyStateAgainWhileTheRoomShowsOther) {
  accept();
  waitingRoom().setReady(true);

  update(READY_STATE_INTERVAL);

  EXPECT_EQ(sentCountOf<network::ReadyState>(), 2U);
}

/**
 * Given a player who asked to be ready
 * When a LobbyState shows the player ready, then two intervals pass
 * Then the ReadyState is not sent again
 */
TEST_F(WaitingRoomFixture, StopsSendingReadyStateOnceTheRoomShowsIt) {
  accept();
  waitingRoom().setReady(true);
  deliverInSession(
      network::LobbyState{.players = {{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                       .ready = true,
                                       .nickname = std::string{NICKNAME}}}},
      FIRST_SEQUENCE);
  update();

  update(2 * READY_STATE_INTERVAL);

  EXPECT_EQ(sentCountOf<network::ReadyState>(), 1U);
}

/**
 * Given a session
 * When the server sends a LobbyState listing two players
 * Then the waiting room is those two players
 */
TEST_F(WaitingRoomFixture, KeepsTheLobbyStateOfTheSession) {
  accept();
  const network::LobbyState lobby{
      .players = {{.playerNumber = OTHER_PLAYER_NUMBER,
                   .ready = true,
                   .nickname = std::string{OTHER_NICKNAME}},
                  {.playerNumber = ACCEPTED_PLAYER_NUMBER,
                   .ready = false,
                   .nickname = std::string{NICKNAME}}}};
  deliverInSession(lobby, FIRST_SEQUENCE);

  update();

  EXPECT_THAT(waitingRoom().lobbyPlayers(), ElementsAreArray(lobby.players));
}

/**
 * Given a session
 * When a LobbyState arrives with another token
 * Then the waiting room still holds the player alone
 */
TEST_F(WaitingRoomFixture, IgnoresALobbyStateWithAnotherToken) {
  accept();
  network::PacketHeader forged = sessionHeader(FIRST_SEQUENCE);
  forged.sessionToken = OTHER_SESSION_TOKEN;
  deliver(network::LobbyState{.players = {{.playerNumber = OTHER_PLAYER_NUMBER,
                                           .ready = true,
                                           .nickname =
                                               std::string{OTHER_NICKNAME}}}},
          server(), forged);

  update();

  ASSERT_EQ(waitingRoom().lobbyPlayers().size(), 1U);
  EXPECT_EQ(waitingRoom().lobbyPlayers().front().playerNumber,
            ACCEPTED_PLAYER_NUMBER);
}

/**
 * Given a session that read the LobbyState of sequence 2
 * When the LobbyState of sequence 1 arrives late
 * Then the waiting room is still the one of sequence 2
 */
TEST_F(WaitingRoomFixture, IgnoresALateLobbyState) {
  accept();
  const network::LobbyState newer{
      .players = {{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                   .ready = true,
                   .nickname = std::string{NICKNAME}}}};
  const network::LobbyState older{
      .players = {{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                   .ready = false,
                   .nickname = std::string{NICKNAME}}}};
  deliverInSession(newer, SECOND_SEQUENCE);
  deliverInSession(older, FIRST_SEQUENCE);

  update();

  EXPECT_THAT(waitingRoom().lobbyPlayers(), ElementsAreArray(newer.players));
}

/**
 * Given a session
 * When the server announces the start of the game
 * Then the connection says the game started
 */
TEST_F(WaitingRoomFixture, LearnsThatTheGameStarted) {
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);

  update();

  EXPECT_TRUE(waitingRoom().gameStarted());
}

/**
 * Given a session whose GameStarted was lost
 * When the first WorldState arrives
 * Then the connection says the game started
 */
TEST_F(WaitingRoomFixture, AWorldStateStartsTheGameWhenGameStartedWasLost) {
  accept();
  deliverWorldStateInSession(stateListing(APPLIED_TICK, {}), FIRST_SEQUENCE);

  update();

  EXPECT_TRUE(waitingRoom().gameStarted());
}

/**
 * Given a player who asked to be ready
 * When the game starts and an interval passes
 * Then the request is forgotten and no ReadyState goes out again
 */
TEST_F(WaitingRoomFixture, ForgetsTheReadyRequestOnceTheGameStarted) {
  accept();
  waitingRoom().setReady(true);
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  update();

  update(READY_STATE_INTERVAL);

  EXPECT_EQ(waitingRoom().requestedReady(), std::nullopt);
  EXPECT_EQ(sentCountOf<network::ReadyState>(), 1U);
}

/**
 * Given a session with a room, a request and a started game
 * When the player leaves it
 * Then the room, the request and the start are forgotten
 */
TEST_F(WaitingRoomFixture, LeavingForgetsTheSession) {
  accept();
  waitingRoom().setReady(true);
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  update();

  session().cancel();

  EXPECT_TRUE(waitingRoom().lobbyPlayers().empty());
  EXPECT_EQ(waitingRoom().requestedReady(), std::nullopt);
  EXPECT_FALSE(waitingRoom().gameStarted());
}

/**
 * Given a game the server ended
 * When the connection is asked whether a game runs
 * Then none does
 */
TEST_F(WaitingRoomFixture, AnEndedGameIsNoLongerStarted) {
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  deliverInSession(endOfOnePilot(), SECOND_SEQUENCE);

  update();

  EXPECT_FALSE(waitingRoom().gameStarted());
}

/**
 * Given a game the server ended
 * When the player asks to be ready again
 * Then a ReadyState is sent
 */
TEST_F(WaitingRoomFixture, SendsReadyAgainAfterAnEndedGame) {
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  deliverInSession(endOfOnePilot(), SECOND_SEQUENCE);
  update();

  waitingRoom().setReady(true);

  EXPECT_EQ(sentCountOf<network::ReadyState>(), 1U);
}

}  // namespace rtype::client

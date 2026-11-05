#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "Disconnection.hpp"
#include "GameStatus.hpp"
#include "Heartbeat.hpp"
#include "SentDatagrams.hpp"
#include "ServerSupervisorLinkFixture.hpp"
#include "SupervisorLinkTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::network::Disconnection;
using rtype::network::GameStatus;
using rtype::network::GameStatusState;
using rtype::network::Heartbeat;
using rtype::server::testing::onlyMessageIn;

/**
 * Given a game process that just started
 * When its first tick runs
 * Then it sends the supervisor a status "waiting for players, 0 players" at
 * once
 */
TEST_F(ServerSupervisorLinkFixture, SendsItsStatusAtTheFirstTick) {
  startWith(shippedWaves());

  runTick({});

  const auto sent = statuses();
  ASSERT_EQ(sent.size(), 1U);
  EXPECT_EQ(onlyMessageIn<GameStatus>(sent.front()),
            (GameStatus{.state = GameStatusState::WaitingForPlayers,
                        .playerCount = 0}));
}

/**
 * Given a game process whose first status went out
 * When 980 milliseconds pass
 * Then it sends no second status
 */
TEST_F(ServerSupervisorLinkFixture, SendsNoSecondStatusBeforeASecond) {
  startWith(shippedWaves());
  runTick({});

  runFor(JUST_UNDER_A_SECOND);

  EXPECT_EQ(statuses().size(), 1U);
}

/**
 * Given a game process whose first status went out
 * When a second passes in all
 * Then it sends a second status
 */
TEST_F(ServerSupervisorLinkFixture, SendsAStatusEverySecond) {
  startWith(shippedWaves());
  runTick({});

  runFor(ONE_SECOND);

  EXPECT_EQ(statuses().size(), 2U);
}

/**
 * Given a game with two players
 * When the next status goes out
 * Then it announces 2 players
 */
TEST_F(ServerSupervisorLinkFixture, AnnouncesTheNumberOfPlayersOfItsGame) {
  startWith(shippedWaves());
  join(0);
  join(1);

  runTick({});

  EXPECT_EQ(onlyMessageIn<GameStatus>(statuses().front()).playerCount, 2);
}

/**
 * Given a game process
 * When the supervisor sends a Disconnection under the token they share
 * Then the program is asked to stop
 */
TEST_F(ServerSupervisorLinkFixture, StopsWhenTheSupervisorSaysDisconnection) {
  startWith(shippedWaves());

  supervisorSays(Disconnection{});

  EXPECT_TRUE(stopWasRequested());
}

/**
 * Given a game process
 * When the supervisor sends a Heartbeat
 * Then the program is not asked to stop
 */
TEST_F(ServerSupervisorLinkFixture,
       KeepsRunningWhenTheSupervisorSaysHeartbeat) {
  startWith(shippedWaves());

  supervisorSays(Heartbeat{});

  EXPECT_FALSE(stopWasRequested());
}

/**
 * Given a game process
 * When 5 seconds pass without a datagram of the supervisor
 * Then the program is asked to stop
 */
TEST_F(ServerSupervisorLinkFixture, StopsWhenTheSupervisorIsSilentFiveSeconds) {
  startWith(shippedWaves());

  runFor(FIVE_SECONDS_AND_A_TICK);

  EXPECT_TRUE(stopWasRequested());
}

/**
 * Given a game process whose supervisor speaks every second
 * When 5 seconds pass
 * Then the program is not asked to stop
 */
TEST_F(ServerSupervisorLinkFixture, KeepsRunningWhileTheSupervisorSpeaks) {
  startWith(shippedWaves());

  for (int second = 0; second < FIVE_SECONDS_OF_BEATS; ++second) {
    supervisorSays(Heartbeat{});
    runFor(ONE_SECOND);
  }

  EXPECT_FALSE(stopWasRequested());
}

/**
 * Given a game process
 * When it started
 * Then the network admitted the supervisor under the shared token
 */
TEST_F(ServerSupervisorLinkFixture, AdmitsTheSupervisorWhenItStarts) {
  startWith(shippedWaves());

  ASSERT_EQ(network().admission().admitted().size(), 1U);
  EXPECT_EQ(network().admission().admitted().front().player, supervisor());
  EXPECT_EQ(network().admission().admitted().front().sessionToken, LINK_TOKEN);
}

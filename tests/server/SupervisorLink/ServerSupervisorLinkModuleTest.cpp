#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Disconnection.hpp"
#include "ExpulsionOrder.hpp"
#include "GameStatus.hpp"
#include "Heartbeat.hpp"
#include "MessageType.hpp"
#include "ModuleConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerSupervisorLinkFixture.hpp"
#include "SupervisorLinkTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::network::Disconnection;
using rtype::network::ExpulsionOrder;
using rtype::network::GameStatus;
using rtype::network::GameStatusState;
using rtype::network::Heartbeat;
using rtype::network::MessageType;
using rtype::server::FAREWELL_COPIES;
using rtype::server::testing::onlyMessageIn;

/**
 * Given a game process that just started
 * When its first tick runs
 * Then it sends the supervisor a status "waiting for players", with no
 * nickname and, without the metrics module, no measure, at once
 */
TEST_F(ServerSupervisorLinkFixture, SendsItsStatusAtTheFirstTick) {
  startWith(shippedWaves());

  runTick({});

  const auto sent = statuses();
  ASSERT_EQ(sent.size(), 1U);
  EXPECT_EQ(onlyMessageIn<GameStatus>(sent.front()),
            (GameStatus{.state = GameStatusState::WaitingForPlayers,
                        .nicknames = {},
                        .measures = {}}));
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
 * Given a game process whose first status went out with no player
 * When a player joins, well before the next second
 * Then a second status goes out at once, announcing the player
 */
TEST_F(ServerSupervisorLinkFixture, SendsAStatusAtOnceWhenAPlayerJoins) {
  startWith(shippedWaves());
  runTick({});
  join(0);

  runTick({});

  const auto sent = statuses();
  ASSERT_EQ(sent.size(), 2U);
  EXPECT_EQ(onlyMessageIn<GameStatus>(sent.back()).nicknames.size(), 1U);
}

/**
 * Given a game with two players
 * When the next status goes out
 * Then it names both, in the order of their numbers
 */
TEST_F(ServerSupervisorLinkFixture,
       AnnouncesTheNicknamesOfThePlayersOfItsGame) {
  startWith(shippedWaves());
  const std::string first = join(0).nickname;
  const std::string second = join(1).nickname;

  runTick({});

  EXPECT_EQ(onlyMessageIn<GameStatus>(statuses().front()).nicknames,
            (std::vector<std::string>{first, second}));
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

/**
 * Given a game process with one pilot seated
 * When the supervisor orders its expulsion twice in one tick, its nickname in
 * capitals
 * Then the pilot gets FAREWELL_COPIES Expulsion datagrams, once
 */
TEST_F(ServerSupervisorLinkFixture, TellsThePilotTheSupervisorExpels) {
  startWith(shippedWaves());
  join(EXPELLED_PILOT_INDEX);
  const ExpulsionOrder order{.nickname =
                                 std::string{SEATED_NICKNAME_IN_CAPITALS}};

  runTick({fromSupervisor(order), fromSupervisor(order)});

  EXPECT_EQ(datagramsTo(EXPELLED_PILOT_INDEX, MessageType::Expulsion),
            FAREWELL_COPIES);
}

/**
 * Given a pilot the supervisor ordered expelled, still seated in that tick
 * When the next tick runs
 * Then its seat is free
 */
TEST_F(ServerSupervisorLinkFixture,
       ClosesTheSessionOfTheExpelledPilotAtTheNextTick) {
  startWith(shippedWaves());
  join(EXPELLED_PILOT_INDEX);
  supervisorSays(
      ExpulsionOrder{.nickname = std::string{SEATED_NICKNAME_IN_CAPITALS}});
  ASSERT_TRUE(isSeated(EXPELLED_PILOT_INDEX));

  runTick({});

  EXPECT_FALSE(isSeated(EXPELLED_PILOT_INDEX));
}

/**
 * Given a game process with one pilot seated
 * When the supervisor orders the expulsion of a nickname no pilot has
 * Then nothing is sent to the pilot, and its seat is kept at the next tick
 */
TEST_F(ServerSupervisorLinkFixture, ExpelsNobodyForAnotherNickname) {
  startWith(shippedWaves());
  join(EXPELLED_PILOT_INDEX);

  supervisorSays(ExpulsionOrder{.nickname = std::string{NICKNAME_NOBODY_HAS}});
  runTick({});

  EXPECT_EQ(datagramsTo(EXPELLED_PILOT_INDEX, MessageType::Expulsion), 0U);
  EXPECT_TRUE(isSeated(EXPELLED_PILOT_INDEX));
}

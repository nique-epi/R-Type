#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include "ConnectionRefused.hpp"
#include "Endpoint.hpp"
#include "GameState.hpp"
#include "GamesSettings.hpp"
#include "MessageType.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyFixture.hpp"
#include "ServerModuleTestConstants.hpp"

using rtype::network::ConnectionRefused;
using rtype::network::MessageType;
using rtype::network::RefusalReason;
using rtype::server::GameState;
using rtype::server::testing::onlyMessageIn;
using rtype::server::testing::playerEndpoint;
using rtype::server::testing::sentTo;

/**
 * Given a game waiting for players
 * When a stranger asks for a place without a cookie
 * Then it gets a challenge, and nobody sits in the game
 */
TEST_F(ServerLobbyFixture, ChallengesAStrangerWhileTheGameWaits) {
  startWith(shippedWaves());

  runTick({}, {requestFrom(0)});

  EXPECT_EQ(datagramsTo(0, MessageType::ConnectionChallenge), 1U);
  EXPECT_TRUE(probe().games()->joinable().roster().endpoints().empty());
}

/**
 * Given a stranger challenged by the server
 * When it asks again with the cookie of the challenge
 * Then it sits in the first game and the network admits its address and token
 */
TEST_F(ServerLobbyFixture, SeatsAPlayerWhoAnswersTheChallengeInTheFirstGame) {
  startWith(shippedWaves());
  runTick({}, {requestFrom(0)});
  const std::uint64_t cookie = challengeCookieOf(0);

  runTick({}, {requestFrom(0, cookie)});

  EXPECT_EQ(probe().games()->joinable().roster().endpoints(),
            (std::vector<rtype::network::Endpoint>{playerEndpoint(0)}));
  ASSERT_EQ(network().admission().admitted().size(), 1U);
  EXPECT_EQ(network().admission().admitted().front().player, playerEndpoint(0));
  EXPECT_EQ(datagramsTo(0, MessageType::ConnectionAccepted), 1U);
}

/**
 * Given a server of one game, which left the waiting room because its only
 * player is ready
 * When a stranger asks for a place
 * Then it is refused with GameInProgress
 */
TEST_F(ServerLobbyFixture, RefusesANewPlayerWhenEveryGameIsInProgress) {
  rtype::server::GamesSettings settings = shippedWaves();
  settings.gameCount = ONE_GAME;
  startWith(std::move(settings));
  join(0);
  sayReady(0);
  ASSERT_EQ(probe().games()->all()[FIRST_GAME]->state(), GameState::InProgress);

  runTick({}, {requestFrom(1)});

  const auto received = sentTo(network().sender().sent(), playerEndpoint(1));
  ASSERT_EQ(received.size(), 1U);
  EXPECT_EQ(onlyMessageIn<ConnectionRefused>(received.front()).reason,
            RefusalReason::GameInProgress);
}

/**
 * Given a first game that left the waiting room because its only player is
 * ready
 * When a stranger answers the challenge of the server
 * Then it sits in the second game instead of being refused
 */
TEST_F(ServerLobbyFixture, SeatsAPlayerWhoArrivesDuringAGameInTheNextGame) {
  startWith(shippedWaves());
  join(0);
  sayReady(0);
  ASSERT_EQ(probe().games()->all()[FIRST_GAME]->state(), GameState::InProgress);

  runTick({}, {requestFrom(1)});
  runTick({}, {requestFrom(1, challengeCookieOf(1))});

  EXPECT_EQ(probe().games()->all()[SECOND_GAME]->roster().endpoints(),
            (std::vector<rtype::network::Endpoint>{playerEndpoint(1)}));
  EXPECT_EQ(datagramsTo(1, MessageType::ConnectionAccepted), 1U);
}

/**
 * Given a player seated in the first game through its challenge, then that
 * game filled up, so new players now join the second one
 * When the player asks again with the same cookie
 * Then its own game answers with the same acceptance again
 */
TEST_F(ServerLobbyFixture, AcceptsAgainAPlayerInItsOwnGame) {
  startWith(shippedWaves());
  runTick({}, {requestFrom(0)});
  const std::uint64_t cookie = challengeCookieOf(0);
  runTick({}, {requestFrom(0, cookie)});
  for (std::size_t index = 1; index < rtype::network::MAXIMUM_PLAYERS;
       ++index) {
    join(index);
  }

  runTick({}, {requestFrom(0, cookie)});

  EXPECT_EQ(datagramsTo(0, MessageType::ConnectionAccepted), 2U);
  EXPECT_EQ(datagramsTo(0, MessageType::ConnectionChallenge), 1U);
}

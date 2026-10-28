#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "ConnectionRefused.hpp"
#include "Endpoint.hpp"
#include "GameState.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "PacketHeader.hpp"
#include "SentDatagrams.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyFixture.hpp"

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
 * Given a game that left the waiting room because its only player is ready
 * When a stranger asks for a place
 * Then it is refused with GameInProgress
 */
TEST_F(ServerLobbyFixture, RefusesANewPlayerOnceTheGameIsInProgress) {
  startWith(shippedWaves());
  join(0);
  rtype::network::PacketHeader header;
  header.sessionToken = tokenOf(0);
  runTick({rtype::network::IncomingDatagram{.sender = playerEndpoint(0),
                                            .payload = readyDatagram(header)}});
  ASSERT_EQ(probe().games()->joinable().state(), GameState::InProgress);

  runTick({}, {requestFrom(1)});

  const auto received = sentTo(network().sender().sent(), playerEndpoint(1));
  ASSERT_EQ(received.size(), 1U);
  EXPECT_EQ(onlyMessageIn<ConnectionRefused>(received.front()).reason,
            RefusalReason::GameInProgress);
}

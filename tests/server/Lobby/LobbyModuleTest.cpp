#include <gtest/gtest.h>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "SentDatagrams.hpp"
#include "ServerDatagrams.hpp"
#include "ServerModulesFixture.hpp"

using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::network::MessageType;
using rtype::server::testing::playerEndpoint;
using rtype::server::testing::ServerModulesFixture;

/**
 * Given the games and the lobby started
 * When a stranger asks to connect without a cookie
 * Then the lobby sends it a ConnectionChallenge, and nothing else
 */
TEST_F(ServerModulesFixture, ChallengesAStrangerWithoutACookie) {
  tick({}, {IncomingDatagram{.sender = playerEndpoint(0),
                             .payload = requestDatagram()}});

  EXPECT_EQ(messageTypesQueuedFor(playerEndpoint(0)),
            std::vector<MessageType>{MessageType::ConnectionChallenge});
}

/**
 * Given the games and the lobby started, and a stranger that got a challenge
 * When the stranger asks again with the cookie of that challenge
 * Then the network admits the stranger's address
 */
TEST_F(ServerModulesFixture, AdmitsAStrangerThatAnsweredItsChallenge) {
  seatPlayer();

  EXPECT_EQ(admittedEndpoints(), std::vector<Endpoint>{playerEndpoint(0)});
}

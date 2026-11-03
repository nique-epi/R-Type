#include <gtest/gtest.h>
#include "Endpoint.hpp"
#include "ServerApplication.hpp"
#include "ServerApplicationTrafficFixtures.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ServerTestConstants.hpp"
#include "TrafficReport.hpp"

using rtype::network::Endpoint;
using rtype::network::TrafficReport;
using rtype::server::ServerApplication;
using rtype::server::testing::OTHER_SESSION_TOKEN;
using rtype::server::testing::SESSION_TOKEN;

/**
 * Given a server that admitted the address of a client
 * When that client sends a datagram
 * Then it counts in the row of that address, not with the others
 */
TEST(ServerApplicationTraffic, CountsAnAdmittedPlayerInARowOfItsOwn) {
  Endpoint player;
  const TrafficReport report = trafficAfterAReadyState(
      [&player](ServerApplication& server, const Endpoint& client) {
        player = client;
        server.admit(client, SESSION_TOKEN);
      });

  ASSERT_EQ(report.peers.size(), 1U);
  EXPECT_EQ(report.peers.front().peer, player);
  EXPECT_EQ(report.peers.front().traffic.received.datagrams.count, 1U);
  EXPECT_EQ(report.others.received.datagrams.count, 0U);
}

/**
 * Given a server that admitted nobody
 * When a stranger sends a datagram
 * Then it counts with the others, and no row is added
 */
TEST(ServerApplicationTraffic, CountsAStrangerWithTheOthers) {
  const TrafficReport report =
      trafficAfterAReadyState([]([[maybe_unused]] ServerApplication& server,
                                 [[maybe_unused]] const Endpoint& client) {});

  EXPECT_TRUE(report.peers.empty());
  EXPECT_EQ(report.others.received.datagrams.count, 1U);
}

/**
 * Given a server that admitted a player, then forgot it under the same token
 * When the address sends a datagram
 * Then the row is gone, and the datagram counts with the others
 */
TEST(ServerApplicationTraffic, AForgottenPlayerLosesItsRow) {
  const TrafficReport report = trafficAfterAReadyState(
      [](ServerApplication& server, const Endpoint& client) {
        server.admit(client, SESSION_TOKEN);
        server.forget(client, SESSION_TOKEN);
      });

  EXPECT_TRUE(report.peers.empty());
  EXPECT_EQ(report.others.received.datagrams.count, 1U);
}

/**
 * Given a player admitted again under a new token, then forgotten under the old
 * one, which is what a session replaced on the same address does
 * When it sends a datagram
 * Then it keeps its row
 */
TEST(ServerApplicationTraffic, ForgettingAReplacedSessionKeepsTheRow) {
  const TrafficReport report = trafficAfterAReadyState(
      [](ServerApplication& server, const Endpoint& client) {
        server.admit(client, SESSION_TOKEN);
        server.admit(client, OTHER_SESSION_TOKEN);
        server.forget(client, SESSION_TOKEN);
      });

  ASSERT_EQ(report.peers.size(), 1U);
  EXPECT_EQ(report.peers.front().traffic.received.datagrams.count, 1U);
}

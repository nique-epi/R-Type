#include <gtest/gtest.h>
#include <vector>
#include "DatagramRoute.hpp"
#include "DatagramRouteFixture.hpp"
#include "DatagramRouterFixtures.hpp"
#include "DatagramRouterTestConstants.hpp"
#include "IncomingDatagram.hpp"

namespace rtype::client::testing {

/**
 * Given a route that was never told whom to listen to
 * When it is asked whether it listens to the game server
 * Then it does not
 */
TEST_F(DatagramRouteFixture, ListensToNobodyAtFirst) {
  EXPECT_FALSE(route().listensTo(gameServer()));
}

/**
 * Given a route told to listen to the game server
 * When it is asked about the game server, then about the master server
 * Then it listens to the first and not to the second, though both share an
 * address
 */
TEST_F(DatagramRouteFixture, ListensToTheEndpointItWasToldTo) {
  route().listenTo(gameServer());

  EXPECT_TRUE(route().listensTo(gameServer()));
  EXPECT_FALSE(route().listensTo(masterServer()));
}

/**
 * Given a route
 * When it is read twice
 * Then the pump was asked once before each read
 */
TEST_F(DatagramRouteFixture, AsksThePumpBeforeEachRead) {
  std::vector<network::IncomingDatagram> read;

  route().drainReceived(read);
  route().drainReceived(read);

  EXPECT_EQ(pump().pumpCount(), 2);
}

/**
 * Given a route told to listen to the game server
 * When it accepts a datagram of that server and is read
 * Then it hands the datagram over
 */
TEST_F(DatagramRouteFixture, HandsOverTheDatagramsItAccepted) {
  route().listenTo(gameServer());
  route().accept(datagramFrom(gameServer(), FIRST_TAG));

  std::vector<network::IncomingDatagram> read;
  route().drainReceived(read);

  EXPECT_EQ(read, datagramsFrom(gameServer(), {FIRST_TAG}));
}

/**
 * Given a route that accepted a datagram
 * When it is told to listen to another sender
 * Then the pump is asked first, so the datagram is no longer waiting
 */
TEST_F(DatagramRouteFixture, AsksThePumpBeforeListeningToAnotherSender) {
  route().accept(datagramFrom(gameServer(), FIRST_TAG));

  route().listenTo(masterServer());

  EXPECT_EQ(pump().pumpCount(), 1);
}

}  // namespace rtype::client::testing

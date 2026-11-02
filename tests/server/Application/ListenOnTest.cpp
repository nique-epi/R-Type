#include <gtest/gtest.h>
#include "ListenOnFixtures.hpp"
#include "NetworkConstants.hpp"
#include "ServerApplication.hpp"
#include "ServerException.hpp"

using rtype::network::ANY_PORT;
using rtype::server::ServerAlreadyOpenException;
using rtype::server::ServerApplication;

/**
 * Given a server built on an address it cannot bind
 * When it listens on the loopback address and opens
 * Then it binds the loopback address
 */
TEST(ServerApplicationListenOn, OpenBindsTheEndpointGivenToListenOn) {
  ServerApplication server{unusableEndpoint(), ignoringTicks()};

  server.listenOn(loopbackEndpoint(ANY_PORT));
  server.open();

  EXPECT_EQ(server.localEndpoint().address, loopbackEndpoint(ANY_PORT).address);
}

/**
 * Given an open server
 * When it is told to listen on another endpoint
 * Then a ServerAlreadyOpenException is thrown
 */
TEST(ServerApplicationListenOn, ListenOnAnOpenServerIsRefused) {
  ServerApplication server{loopbackEndpoint(ANY_PORT), ignoringTicks()};
  server.open();

  EXPECT_THROW(server.listenOn(loopbackEndpoint(ANY_PORT)),
               ServerAlreadyOpenException);
}

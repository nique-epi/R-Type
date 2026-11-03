#include <gtest/gtest.h>
#include <vector>
#include "DatagramRoute.hpp"
#include "DatagramRouterFixture.hpp"
#include "DatagramRouterFixtures.hpp"
#include "DatagramRouterTestConstants.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::client::testing {

/**
 * Given a route listening to the game server and another to the master
 * server, and datagrams of both arriving mixed
 * When each route is read
 * Then each one hands over the datagrams of its own sender only, in the order
 * they arrived
 */
TEST_F(DatagramRouterFixture, HandsEachRouteTheDatagramsOfItsSender) {
  DatagramRoute& game = router().addRoute();
  DatagramRoute& master = router().addRoute();
  game.listenTo(gameServer());
  master.listenTo(masterServer());
  arrive(datagramFrom(gameServer(), FIRST_TAG));
  arrive(datagramFrom(masterServer(), SECOND_TAG));
  arrive(datagramFrom(gameServer(), THIRD_TAG));

  const std::vector<network::IncomingDatagram> readByGame = readBy(game);
  const std::vector<network::IncomingDatagram> readByMaster = readBy(master);

  EXPECT_EQ(readByGame, datagramsFrom(gameServer(), {FIRST_TAG, THIRD_TAG}));
  EXPECT_EQ(readByMaster, datagramsFrom(masterServer(), {SECOND_TAG}));
}

/**
 * Given two routes, the first of which empties the socket on its read
 * When the second route is read afterwards
 * Then it still hands over the datagram of its sender
 */
TEST_F(DatagramRouterFixture, KeepsForTheOtherRouteWhatTheFirstReadDrained) {
  DatagramRoute& game = router().addRoute();
  DatagramRoute& master = router().addRoute();
  game.listenTo(gameServer());
  master.listenTo(masterServer());
  arrive(datagramFrom(masterServer(), FIRST_TAG));
  EXPECT_TRUE(readBy(game).empty());

  EXPECT_EQ(readBy(master), datagramsFrom(masterServer(), {FIRST_TAG}));
}

/**
 * Given a route listening to the game server
 * When a datagram of a host nobody listens to arrives
 * Then no route hands it over
 */
TEST_F(DatagramRouterFixture, DropsADatagramNobodyListensTo) {
  DatagramRoute& game = router().addRoute();
  game.listenTo(gameServer());
  arrive(datagramFrom(stranger(), FIRST_TAG));

  EXPECT_TRUE(readBy(game).empty());
}

/**
 * Given a route that was never told whom to listen to
 * When a datagram of the game server arrives
 * Then the route hands nothing over
 */
TEST_F(DatagramRouterFixture, HandsNothingToARouteThatListensToNobody) {
  DatagramRoute& route = router().addRoute();
  arrive(datagramFrom(gameServer(), FIRST_TAG));

  EXPECT_TRUE(readBy(route).empty());
}

/**
 * Given a route that read a datagram
 * When it is read again with nothing new arrived
 * Then it hands over nothing: a datagram is handed over once
 */
TEST_F(DatagramRouterFixture, HandsOverADatagramOnce) {
  DatagramRoute& game = router().addRoute();
  game.listenTo(gameServer());
  arrive(datagramFrom(gameServer(), FIRST_TAG));
  EXPECT_EQ(readBy(game).size(), 1);

  EXPECT_TRUE(readBy(game).empty());
}

/**
 * Given a route listening to the game server with a datagram of it waiting in
 * the socket
 * When the route is told to listen to the master server
 * Then the datagram of the game server is never handed over, and the next
 * datagram of the master server is
 */
TEST_F(DatagramRouterFixture, ListeningToAnotherSenderDropsWhatWasWaiting) {
  DatagramRoute& route = router().addRoute();
  route.listenTo(gameServer());
  arrive(datagramFrom(gameServer(), FIRST_TAG));

  route.listenTo(masterServer());
  arrive(datagramFrom(masterServer(), SECOND_TAG));

  EXPECT_EQ(readBy(route), datagramsFrom(masterServer(), {SECOND_TAG}));
}

/**
 * Given a route and a datagram of the master server waiting in the socket
 * When the route is told to listen to the master server
 * Then that datagram, sent before, is not handed over: a late answer of an
 * earlier exchange never reaches the new one
 */
TEST_F(DatagramRouterFixture, ListeningToASenderDropsItsDatagramsOfBefore) {
  DatagramRoute& route = router().addRoute();
  arrive(datagramFrom(masterServer(), FIRST_TAG));

  route.listenTo(masterServer());
  arrive(datagramFrom(masterServer(), SECOND_TAG));

  EXPECT_EQ(readBy(route), datagramsFrom(masterServer(), {SECOND_TAG}));
}

/**
 * Given two routes, the second with a datagram of its sender waiting
 * When the first route is told to listen to another sender
 * Then the second route still hands its datagram over
 */
TEST_F(DatagramRouterFixture,
       ListeningToAnotherSenderLeavesTheOtherRoutesAlone) {
  DatagramRoute& game = router().addRoute();
  DatagramRoute& master = router().addRoute();
  master.listenTo(masterServer());
  arrive(datagramFrom(masterServer(), FIRST_TAG));

  game.listenTo(gameServer());

  EXPECT_EQ(readBy(master), datagramsFrom(masterServer(), {FIRST_TAG}));
}

/**
 * Given a route
 * When a datagram is sent through it
 * Then the socket sends that datagram, to the destination it names
 */
TEST_F(DatagramRouterFixture, SendsThroughTheSocket) {
  DatagramRoute& route = router().addRoute();

  route.send(datagramTo(masterServer(), FIRST_TAG));

  EXPECT_EQ(socket().sent(), std::vector<network::OutgoingDatagram>{
                                 datagramTo(masterServer(), FIRST_TAG)});
}

}  // namespace rtype::client::testing

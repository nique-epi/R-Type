#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <future>
#include <span>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "NetworkException.hpp"
#include "NetworkTestConstants.hpp"
#include "NetworkThread.hpp"
#include "NetworkThreadFixtures.hpp"
#include "NetworkThreadTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "UdpPortProbe.hpp"
#include "UdpSocket.hpp"

using rtype::client::NetworkThread;
using rtype::network::ANY_PORT;
using rtype::network::Endpoint;
using rtype::network::IncomingDatagram;
using rtype::network::NetworkContext;
using rtype::network::OutgoingDatagram;
using rtype::network::UdpSocket;

/**
 * Given a network thread opened on the loopback address
 * When a peer sends it a datagram
 * Then draining it on the test thread returns that datagram, with the peer as
 * its sender
 */
TEST(NetworkThread, HandsEachReceivedDatagramToTheDrainingThread) {
  NetworkThread networkThread{loopbackOnPort(ANY_PORT)};
  networkThread.open();
  NetworkContext peerContext;
  UdpSocket peer{peerContext, loopbackOnPort(ANY_PORT)};

  peer.send(networkThread.localEndpoint(), exchangedBytes());
  peerContext.run();
  const std::vector<IncomingDatagram> received =
      drainUntilReceived(networkThread);

  EXPECT_EQ(received, (std::vector<IncomingDatagram>{IncomingDatagram{
                          .sender = loopbackOnPort(peer.localEndpoint().port),
                          .payload = exchangedBytes()}}));
}

/**
 * Given a network thread opened and a peer listening on the loopback address
 * When the test thread hands the network thread a datagram for the peer
 * Then the peer receives its bytes, sent from the network thread's port
 */
TEST(NetworkThread, SendsEachHandedDatagramToItsDestination) {
  NetworkThread networkThread{loopbackOnPort(ANY_PORT)};
  networkThread.open();
  NetworkContext peerContext;
  UdpSocket peer{peerContext, loopbackOnPort(ANY_PORT)};
  const Endpoint peerEndpoint = peer.localEndpoint();
  std::promise<IncomingDatagram> arrival;
  std::future<IncomingDatagram> arrived = arrival.get_future();
  peer.startReceiving(
      [&arrival, &peerContext](const Endpoint& sender,
                               std::span<const std::byte> payload) {
        arrival.set_value(IncomingDatagram{
            .sender = sender, .payload = {payload.begin(), payload.end()}});
        peerContext.stop();
      });
  const std::jthread peerThread{[&peerContext] { peerContext.run(); }};

  networkThread.send(OutgoingDatagram{.destination = peerEndpoint,
                                      .payload = exchangedBytes()});
  const std::future_status status = arrived.wait_for(DATAGRAM_TIMEOUT);
  peerContext.stop();

  ASSERT_EQ(status, std::future_status::ready);
  EXPECT_EQ(arrived.get(),
            (IncomingDatagram{.sender = networkThread.localEndpoint(),
                              .payload = exchangedBytes()}));
}

/**
 * Given SENDING_THREADS_DESTROYED_AT_ONCE network threads, opened one after
 * the other, each handed one datagram for a peer
 * When each one is destroyed right after its datagram was handed over
 * Then every datagram reaches the peer
 */
TEST(NetworkThread, SendsWhatWaitsBeforeItStops) {
  NetworkContext peerContext;
  UdpSocket peer{peerContext, loopbackOnPort(ANY_PORT)};
  const Endpoint peerEndpoint = peer.localEndpoint();
  std::size_t arrivals = 0;
  std::promise<void> everyArrival;
  const std::future<void> arrivedAll = everyArrival.get_future();
  peer.startReceiving([&arrivals, &everyArrival, &peerContext](
                          [[maybe_unused]] const Endpoint& sender,
                          [[maybe_unused]] std::span<const std::byte> payload) {
    ++arrivals;
    if (arrivals == SENDING_THREADS_DESTROYED_AT_ONCE) {
      everyArrival.set_value();
      peerContext.stop();
    }
  });
  const std::jthread peerThread{[&peerContext] { peerContext.run(); }};

  for (std::size_t created = 0; created < SENDING_THREADS_DESTROYED_AT_ONCE;
       ++created) {
    NetworkThread networkThread{loopbackOnPort(ANY_PORT)};
    networkThread.open();
    networkThread.send(OutgoingDatagram{.destination = peerEndpoint,
                                        .payload = exchangedBytes()});
  }
  const std::future_status status = arrivedAll.wait_for(DATAGRAM_TIMEOUT);
  peerContext.stop();

  EXPECT_EQ(status, std::future_status::ready);
}

/**
 * Given a network thread opened that received nothing
 * When the test thread drains it
 * Then the call returns without adding anything
 */
TEST(NetworkThread, DrainingReturnsAtOnceWhenNothingArrived) {
  NetworkThread networkThread{loopbackOnPort(ANY_PORT)};
  networkThread.open();
  std::vector<IncomingDatagram> received;

  networkThread.drainReceived(received);

  EXPECT_TRUE(received.empty());
}

/**
 * Given a network thread opened
 * When it is handed a datagram larger than MAX_DATAGRAM_SIZE
 * Then the network thread stops, and draining it throws
 * DatagramTooLargeException
 */
TEST(NetworkThread, DrainingThrowsWhatStoppedTheNetworkThread) {
  NetworkThread networkThread{loopbackOnPort(ANY_PORT)};
  networkThread.open();

  networkThread.send(OutgoingDatagram{
      .destination = networkThread.localEndpoint(),
      .payload = std::vector<std::byte>(OVERSIZED_PAYLOAD_SIZE)});

  EXPECT_THROW(drainUntilReceived(networkThread),
               rtype::network::DatagramTooLargeException);
}

/**
 * Given network threads opened then destroyed at once, some before their
 * thread ran the network
 * When each one is destroyed
 * Then its destructor returns, instead of waiting for a network that was told
 * to stop before it started
 */
TEST(NetworkThread, StopsEvenWhenDestroyedBeforeItsThreadRuns) {
  for (int created = 0; created < IMMEDIATE_DESTRUCTION_COUNT; ++created) {
    NetworkThread networkThread{loopbackOnPort(ANY_PORT)};
    networkThread.open();
  }

  SUCCEED();
}

/**
 * Given a network thread built on a UDP port nothing is bound to
 * When it is not opened
 * Then the port is still free
 */
TEST(NetworkThread, BindsNothingBeforeItIsOpened) {
  const std::uint16_t port = freeUdpPort();

  const NetworkThread networkThread{anyInterfaceOn(port)};

  EXPECT_TRUE(udpPortIsFree(port));
}

/**
 * Given a network thread opened on every IPv4 interface, on the UDP port the
 * system picked
 * When it is closed
 * Then that port is free again
 */
TEST(NetworkThread, FreesItsPortOnceClosed) {
  NetworkThread networkThread{anyInterfaceOn(ANY_PORT)};
  networkThread.open();
  const std::uint16_t port = networkThread.localEndpoint().port;

  networkThread.close();

  EXPECT_TRUE(udpPortIsFree(port));
}

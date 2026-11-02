#include <gtest/gtest.h>
#include <memory>
#include "DatagramSocketOpening.hpp"
#include "DatagramsInFlightTestConstants.hpp"
#include "DebugJournal.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "NetworkException.hpp"
#include "NetworkSimulatorTestConstants.hpp"
#include "SimulatedNetworkSocket.hpp"
#include "SimulatorFixtures.hpp"
#include "SimulatorTestConstants.hpp"
#include "UdpSocket.hpp"

using rtype::network::ANY_PORT;
using rtype::network::IDatagramSocket;
using rtype::network::NetworkConditions;
using rtype::network::NetworkContext;
using rtype::network::openDatagramSocket;
using rtype::network::SimulatedDelayOutOfRangeException;
using rtype::network::SimulatedNetworkSocket;
using rtype::network::UdpSocket;

/**
 * Given a perfect network
 * When a program opens its socket
 * Then it gets the UDP socket itself, with nothing in between
 */
TEST(DatagramSocketOpening, OpensTheUdpSocketItselfOverAPerfectNetwork) {
  NetworkContext context;

  const std::unique_ptr<IDatagramSocket> socket = openDatagramSocket(
      context, loopbackOnPort(ANY_PORT), NetworkConditions{});

  EXPECT_NE(dynamic_cast<UdpSocket*>(socket.get()), nullptr);
}

/**
 * Given a network with a lag
 * When a program opens its socket
 * Then it gets a simulated socket
 */
TEST(DatagramSocketOpening, OpensASimulatedSocketOverAnImperfectNetwork) {
  NetworkContext context;

  const std::unique_ptr<IDatagramSocket> socket =
      openDatagramSocket(context, loopbackOnPort(ANY_PORT), LAGGING_NETWORK);

  EXPECT_NE(dynamic_cast<SimulatedNetworkSocket*>(socket.get()), nullptr);
}

/**
 * Given a network with a lag
 * When a program opens its socket
 * Then the journal says once that the network is simulated
 */
TEST(DatagramSocketOpening, AnnouncesTheSimulatedNetworkInTheJournal) {
  const DebugJournal journal;
  NetworkContext context;

  static_cast<void>(
      openDatagramSocket(context, loopbackOnPort(ANY_PORT), LAGGING_NETWORK));

  EXPECT_EQ(journal.linesHolding(SIMULATED_NETWORK_ANNOUNCEMENT).size(),
            ONE_ANNOUNCEMENT);
}

/**
 * Given a network whose lag is one millisecond below zero
 * When a program opens its socket
 * Then the opening is refused, and the journal never says the network is
 *      simulated
 */
TEST(DatagramSocketOpening, AnnouncesNoSimulatedNetworkItRefuses) {
  const DebugJournal journal;
  NetworkContext context;

  EXPECT_THROW(static_cast<void>(openDatagramSocket(
                   context, loopbackOnPort(ANY_PORT), LAG_JUST_BELOW_ZERO)),
               SimulatedDelayOutOfRangeException);

  EXPECT_TRUE(journal.linesHolding(SIMULATED_NETWORK_ANNOUNCEMENT).empty());
}

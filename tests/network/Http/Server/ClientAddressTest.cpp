#include <gtest/gtest.h>
#include <string>
#include "ClientAddress.hpp"
#include "ClientAddressTestConstants.hpp"
#include "ForwardedAddress.hpp"
#include "PeerAddresses.hpp"

using rtype::network::clientAddressOf;
using rtype::network::ForwardedAddress;
using rtype::network::PeerAddresses;

/**
 * Given a request from the proxy on the machine, naming one client
 * When a server that trusts that proxy reads its client
 * Then it is the client the proxy names
 */
TEST(ClientAddress, ATrustedProxyNamesTheClient) {
  EXPECT_EQ(
      clientAddressOf(
          PeerAddresses{.peer = std::string{PROXY_ON_THE_MACHINE},
                        .forwardedFor = std::string{ONE_FORWARDED_CLIENT}},
          ForwardedAddress::TrustedFromLoopback),
      ONE_FORWARDED_CLIENT);
}

/**
 * Given a request from the proxy whose client sent an address of its own
 * When a server that trusts that proxy reads its client
 * Then it is the last address, the one the proxy saw, without its blanks
 */
TEST(ClientAddress, TheProxyAppendsTheClientItSawLast) {
  EXPECT_EQ(clientAddressOf(
                PeerAddresses{.peer = std::string{PROXY_ON_THE_MACHINE},
                              .forwardedFor =
                                  std::string{CHAIN_ENDING_IN_THE_SEEN_CLIENT}},
                ForwardedAddress::TrustedFromLoopback),
            SEEN_CLIENT);
}

/**
 * Given a request that reaches the server from another machine, naming a
 * client
 * When a server that trusts the proxy on its machine reads its client
 * Then it is the TCP peer, since only the loopback is that proxy
 */
TEST(ClientAddress, AClientFromElsewhereCannotNameAnother) {
  EXPECT_EQ(
      clientAddressOf(
          PeerAddresses{.peer = std::string{DIRECT_CLIENT},
                        .forwardedFor = std::string{ONE_FORWARDED_CLIENT}},
          ForwardedAddress::TrustedFromLoopback),
      DIRECT_CLIENT);
}

/**
 * Given a request from the loopback naming a client
 * When a server that trusts no proxy reads its client
 * Then it is the loopback
 */
TEST(ClientAddress, AServerThatTrustsNoProxyReadsThePeer) {
  EXPECT_EQ(
      clientAddressOf(
          PeerAddresses{.peer = std::string{PROXY_ON_THE_MACHINE},
                        .forwardedFor = std::string{ONE_FORWARDED_CLIENT}},
          ForwardedAddress::Ignored),
      PROXY_ON_THE_MACHINE);
}

/**
 * Given a request from the proxy whose last entry names no address
 * When a server that trusts that proxy reads its client
 * Then it is the TCP peer, never an empty address
 */
TEST(ClientAddress, ABlankLastEntryLeavesThePeer) {
  EXPECT_EQ(
      clientAddressOf(
          PeerAddresses{.peer = std::string{PROXY_ON_THE_MACHINE},
                        .forwardedFor = std::string{CHAIN_ENDING_IN_A_BLANK}},
          ForwardedAddress::TrustedFromLoopback),
      PROXY_ON_THE_MACHINE);
}

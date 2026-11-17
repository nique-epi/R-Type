#include <gtest/gtest.h>
#include <string>
#include "ClientAddress.hpp"
#include "ClientAddressTestConstants.hpp"
#include "ForwardedAddress.hpp"
#include "PeerAddresses.hpp"

using rtype::network::clientAddressOf;
using rtype::network::ForwardedAddress;
using rtype::network::PeerAddresses;
using rtype::network::reachedOverHttps;

/**
 * Given a request from the proxy on the machine, naming one client
 * When a server that trusts that proxy reads its client
 * Then it is the client the proxy names
 */
TEST(ClientAddress, ATrustedProxyNamesTheClient) {
  EXPECT_EQ(clientAddressOf(
                PeerAddresses{.peer = std::string{PROXY_ON_THE_MACHINE},
                              .forwardedFor = std::string{ONE_FORWARDED_CLIENT},
                              .forwardedProto = {}},
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
                                  std::string{CHAIN_ENDING_IN_THE_SEEN_CLIENT},
                              .forwardedProto = {}},
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
  EXPECT_EQ(clientAddressOf(
                PeerAddresses{.peer = std::string{DIRECT_CLIENT},
                              .forwardedFor = std::string{ONE_FORWARDED_CLIENT},
                              .forwardedProto = {}},
                ForwardedAddress::TrustedFromLoopback),
            DIRECT_CLIENT);
}

/**
 * Given a request from the loopback naming a client
 * When a server that trusts no proxy reads its client
 * Then it is the loopback
 */
TEST(ClientAddress, AServerThatTrustsNoProxyReadsThePeer) {
  EXPECT_EQ(clientAddressOf(
                PeerAddresses{.peer = std::string{PROXY_ON_THE_MACHINE},
                              .forwardedFor = std::string{ONE_FORWARDED_CLIENT},
                              .forwardedProto = {}},
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
                        .forwardedFor = std::string{CHAIN_ENDING_IN_A_BLANK},
                        .forwardedProto = {}},
          ForwardedAddress::TrustedFromLoopback),
      PROXY_ON_THE_MACHINE);
}

/**
 * Given a request from the proxy on the machine, which says its client came
 * over HTTPS
 * When a server that trusts that proxy reads how it came
 * Then it came over HTTPS
 */
TEST(ClientAddress, ATrustedProxyTellsAClientCameOverHttps) {
  EXPECT_TRUE(reachedOverHttps(
      PeerAddresses{.peer = std::string{PROXY_ON_THE_MACHINE},
                    .forwardedFor = std::string{ONE_FORWARDED_CLIENT},
                    .forwardedProto = std::string{HTTPS_IN_CAPITALS}},
      ForwardedAddress::TrustedFromLoopback));
}

/**
 * Given a request from the proxy on the machine, whose client came in plain
 * HTTP
 * When a server that trusts that proxy reads how it came
 * Then it did not come over HTTPS
 */
TEST(ClientAddress, PlainHttpThroughTheProxyIsNotHttps) {
  EXPECT_FALSE(reachedOverHttps(
      PeerAddresses{.peer = std::string{PROXY_ON_THE_MACHINE},
                    .forwardedFor = std::string{ONE_FORWARDED_CLIENT},
                    .forwardedProto = std::string{PLAIN_HTTP}},
      ForwardedAddress::TrustedFromLoopback));
}

/**
 * Given a request that reaches the server from another machine and says it
 * came over HTTPS
 * When a server that trusts the proxy on its machine reads how it came
 * Then it did not come over HTTPS, since the server speaks no TLS itself
 */
TEST(ClientAddress, AClientFromElsewhereCannotClaimHttps) {
  EXPECT_FALSE(reachedOverHttps(
      PeerAddresses{.peer = std::string{DIRECT_CLIENT},
                    .forwardedFor = std::string{ONE_FORWARDED_CLIENT},
                    .forwardedProto = std::string{HTTPS_IN_CAPITALS}},
      ForwardedAddress::TrustedFromLoopback));
}

/**
 * Given a request from the loopback that says it came over HTTPS
 * When a server that trusts no proxy reads how it came
 * Then it did not come over HTTPS
 */
TEST(ClientAddress, AServerThatTrustsNoProxyNeverSeesHttps) {
  EXPECT_FALSE(reachedOverHttps(
      PeerAddresses{.peer = std::string{PROXY_ON_THE_MACHINE},
                    .forwardedFor = std::string{ONE_FORWARDED_CLIENT},
                    .forwardedProto = std::string{HTTPS_IN_CAPITALS}},
      ForwardedAddress::Ignored));
}

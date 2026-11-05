#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ConnectionRequest.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "RequestReading.hpp"
#include "RequestReadingTestConstants.hpp"
#include "ServerDatagrams.hpp"

using rtype::network::ConnectionRequest;
using rtype::server::firstConnectionRequest;
using rtype::server::isOfAnotherVersion;

/**
 * Given a datagram holding a connection request that carries a cookie
 * When its first request is read
 * Then the request holds that cookie
 */
TEST(RequestReading, ReadsTheRequestOfADatagram) {
  const std::vector<std::byte> datagram = requestDatagram(SOME_REQUEST_COOKIE);

  const auto request = firstConnectionRequest(datagram);

  ASSERT_TRUE(request.has_value());
  EXPECT_EQ(request.value_or(ConnectionRequest{}).cookie, SOME_REQUEST_COOKIE);
}

/**
 * Given a datagram that holds a heartbeat and no request
 * When its first request is read
 * Then there is none
 */
TEST(RequestReading, FindsNoRequestInADatagramWithoutOne) {
  EXPECT_FALSE(firstConnectionRequest(heartbeatDatagram()).has_value());
}

/**
 * Given a datagram of another protocol version
 * When it is asked whether it is of another version
 * Then it is
 */
TEST(RequestReading, RecognizesADatagramOfAnotherVersion) {
  rtype::network::PacketHeader header;
  header.version = rtype::network::PROTOCOL_VERSION + 1;

  EXPECT_TRUE(isOfAnotherVersion(heartbeatDatagram(header)));
}

/**
 * Given a datagram of this protocol version
 * When it is asked whether it is of another version
 * Then it is not
 */
TEST(RequestReading, ADatagramOfThisVersionIsNotOfAnotherVersion) {
  EXPECT_FALSE(isOfAnotherVersion(requestDatagram()));
}

/**
 * Given a datagram one byte shorter than a header
 * When it is asked whether it is of another version
 * Then it is not, and nothing is thrown
 */
TEST(RequestReading, ATooShortDatagramIsNotOfAnotherVersion) {
  const std::vector<std::byte> datagram(rtype::network::PACKET_HEADER_SIZE - 1);

  EXPECT_FALSE(isOfAnotherVersion(datagram));
}

/**
 * Given a datagram one byte shorter than a header
 * When its first request is read
 * Then a malformed datagram is reported
 */
TEST(RequestReading, RefusesADatagramShorterThanAHeader) {
  const std::vector<std::byte> datagram(rtype::network::PACKET_HEADER_SIZE - 1);

  EXPECT_THROW(static_cast<void>(firstConnectionRequest(datagram)),
               rtype::network::MalformedDatagramException);
}

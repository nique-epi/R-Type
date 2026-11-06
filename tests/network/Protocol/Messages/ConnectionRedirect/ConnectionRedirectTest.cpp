#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ByteSequence.hpp"
#include "ConnectionRedirect.hpp"
#include "ConnectionRedirectTestConstants.hpp"
#include "DatagramCodec.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network::testing {

/**
 * Given a redirect to port 0x1234
 * When it is encoded
 * Then the payload is the port on two bytes, the most significant first
 */
TEST(ConnectionRedirect, EncodesThePortOnTwoBytesMostSignificantFirst) {
  EXPECT_EQ(encodeMessage(ConnectionRedirect{.port = REDIRECT_PORT}),
            byteSequence({0x12, 0x34}));
}

/**
 * Given a redirect to port 0x1234
 * When it is encoded then decoded
 * Then the decoded redirect equals the original
 */
TEST(ConnectionRedirect, DecodesWhatItEncoded) {
  const ConnectionRedirect redirect{.port = REDIRECT_PORT};

  EXPECT_EQ(decodeMessage<ConnectionRedirect>(encodeMessage(redirect)),
            redirect);
}

/**
 * Given a redirect to port 0
 * When it is encoded
 * Then the encoding is refused, since no player can reach port 0
 */
TEST(ConnectionRedirect, RefusesToEncodePortZero) {
  EXPECT_THROW(static_cast<void>(
                   encodeMessage(ConnectionRedirect{.port = UNREACHABLE_PORT})),
               InvalidFieldValueException);
}

/**
 * Given a payload that holds port 0
 * When it is decoded
 * Then it is refused
 */
TEST(ConnectionRedirect, RefusesToDecodePortZero) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ConnectionRedirect>(
                   byteSequence({0x00, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a payload of one byte
 * When it is decoded
 * Then it is refused as too short
 */
TEST(ConnectionRedirect, RefusesAPayloadOfOneByte) {
  EXPECT_THROW(static_cast<void>(
                   decodeMessage<ConnectionRedirect>(byteSequence({0x12}))),
               BufferUnderflowException);
}

/**
 * Given a payload of three bytes
 * When it is decoded
 * Then it is refused, since a byte follows the port
 */
TEST(ConnectionRedirect, RefusesAByteAfterThePort) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ConnectionRedirect>(
                   byteSequence({0x12, 0x34, 0x00}))),
               TrailingBytesException);
}

/**
 * Given a redirect in a datagram with a header of its own
 * When the datagram is encoded
 * Then it holds CONNECTION_REDIRECT_DATAGRAM_SIZE bytes, never more than the
 * smallest connection request it answers
 */
TEST(ConnectionRedirect, FitsInWhatTheSmallestRequestSent) {
  const std::vector<std::byte> datagram = DatagramCodec::encode(
      PacketHeader{}, ConnectionRedirect{.port = REDIRECT_PORT});

  EXPECT_EQ(datagram.size(), CONNECTION_REDIRECT_DATAGRAM_SIZE);
  EXPECT_LE(datagram.size(), SMALLEST_CONNECTION_REQUEST_DATAGRAM_SIZE);
}

}  // namespace rtype::network::testing

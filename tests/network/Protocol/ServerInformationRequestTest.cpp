#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "DatagramCodec.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "ServerInformationRequest.hpp"
#include "ServerInformationRequestTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given a request sent at 0x01020304
 * When it is encoded
 * Then the payload is sentAt in big-endian, then the 77 zero bytes of the
 * padding the protocol page fixes
 */
TEST(ServerInformationRequest, WritesSentAtThenZeroPadding) {
  std::vector<std::byte> expected{std::byte{0x01}, std::byte{0x02},
                                  std::byte{0x03}, std::byte{0x04}};
  expected.resize(SENT_AT_BYTES + PADDING_ON_THE_PROTOCOL_PAGE, std::byte{0});

  EXPECT_EQ(encodeMessage(ServerInformationRequest{.sentAt = SOME_SENT_AT}),
            expected);
}

/**
 * Given a request in a datagram with a header of no session
 * When the datagram is encoded
 * Then it holds 106 bytes, as the protocol page adds them up
 */
TEST(ServerInformationRequest, FillsADatagramOfTheSizeOfTheProtocolPage) {
  const std::vector<std::byte> datagram = DatagramCodec::encode(
      PacketHeader{}, ServerInformationRequest{.sentAt = SOME_SENT_AT});

  EXPECT_EQ(datagram.size(), REQUEST_DATAGRAM_ON_THE_PROTOCOL_PAGE);
}

/**
 * Given an encoded request
 * When it is decoded
 * Then it is the request that was encoded
 */
TEST(ServerInformationRequest, ComesBackTheSame) {
  const ServerInformationRequest request{.sentAt = SOME_SENT_AT};

  EXPECT_EQ(decodeMessage<ServerInformationRequest>(encodeMessage(request)),
            request);
}

/**
 * Given a request whose padding lacks its last byte
 * When it is decoded
 * Then it is refused: it is too short to pay for its answer
 */
TEST(ServerInformationRequest, RefusesARequestOneByteShort) {
  std::vector<std::byte> payload =
      encodeMessage(ServerInformationRequest{.sentAt = SOME_SENT_AT});
  payload.pop_back();

  EXPECT_THROW((void)decodeMessage<ServerInformationRequest>(payload),
               BufferUnderflowException);
}

/**
 * Given a request with one byte after its padding
 * When it is decoded
 * Then it is refused for its trailing byte
 */
TEST(ServerInformationRequest, RefusesAByteAfterThePadding) {
  std::vector<std::byte> payload =
      encodeMessage(ServerInformationRequest{.sentAt = SOME_SENT_AT});
  payload.push_back(std::byte{0});

  EXPECT_THROW((void)decodeMessage<ServerInformationRequest>(payload),
               TrailingBytesException);
}

/**
 * Given a request whose last byte of padding is not 0
 * When it is decoded
 * Then it is refused for its padding
 */
TEST(ServerInformationRequest, RefusesAPaddingThatIsNotZero) {
  std::vector<std::byte> payload =
      encodeMessage(ServerInformationRequest{.sentAt = SOME_SENT_AT});
  payload.back() = std::byte{NONZERO_PADDING_BYTE};

  EXPECT_THROW((void)decodeMessage<ServerInformationRequest>(payload),
               InvalidFieldValueException);
}

}  // namespace rtype::network::testing

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ConnectionAcceptedFixtures.hpp"
#include "ConnectionAcceptedTestConstants.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given the acceptance of player 1 announcing a pack of a known fingerprint,
 * size and port
 * When it is encoded
 * Then the payload is the player number, the fingerprint, the size and the
 * port, in network byte order, as the protocol page writes them
 */
TEST(ConnectionAccepted, EncodesThePlayerNumberThenTheContent) {
  EXPECT_EQ(encodeMessage(acceptanceOfAPackOf(SOME_PACK_SIZE)),
            payloadOnTheProtocolPage());
}

/**
 * Given an acceptance
 * When it is encoded
 * Then it takes the bytes the protocol page states, whatever it announces
 */
TEST(ConnectionAccepted, TakesTheSizeTheProtocolPageStates) {
  EXPECT_EQ(encodeMessage(acceptanceOfAPackOf(SOME_PACK_SIZE)).size(),
            ACCEPTANCE_PAYLOAD_BYTES);
}

/**
 * Given an acceptance announcing the largest pack
 * When it is encoded then decoded
 * Then the decoded acceptance equals the original
 */
TEST(ConnectionAccepted, DecodesWhatItEncoded) {
  const ConnectionAccepted acceptance = acceptanceOfAPackOf(LARGEST_PACK_SIZE);

  EXPECT_EQ(decodeMessage<ConnectionAccepted>(encodeMessage(acceptance)),
            acceptance);
}

/**
 * Given the acceptances of players 0 and 5
 * When each is encoded, and its payload written by hand decoded
 * Then both are refused
 */
TEST(ConnectionAccepted, RefusesAPlayerNumberOutsideOneToFour) {
  for (const std::uint8_t number :
       {PLAYER_NUMBER_BELOW_THE_FIRST, PLAYER_NUMBER_ABOVE_THE_LAST}) {
    ConnectionAccepted acceptance = acceptanceOfAPackOf(SOME_PACK_SIZE);
    acceptance.playerNumber = number;
    std::vector<std::byte> payload = payloadOnTheProtocolPage();
    payload.front() = std::byte{number};

    EXPECT_THROW((void)encodeMessage(acceptance), InvalidFieldValueException);
    EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(payload),
                 InvalidFieldValueException);
  }
}

/**
 * Given the last player, number 4
 * When its acceptance is decoded
 * Then it gives that number
 */
TEST(ConnectionAccepted, ReadsTheLastPlayerNumber) {
  std::vector<std::byte> payload = payloadOnTheProtocolPage();
  payload.front() = std::byte{LAST_PLAYER_NUMBER};

  EXPECT_EQ(decodeMessage<ConnectionAccepted>(payload).playerNumber,
            LAST_PLAYER_NUMBER);
}

/**
 * Given an acceptance announcing one byte more than the largest pack
 * When it is encoded, and its payload decoded
 * Then both are refused, so a client never waits for more than a pack holds
 */
TEST(ConnectionAccepted, RefusesAPackLargerThanAPackMayBe) {
  ConnectionAccepted acceptance = acceptanceOfAPackOf(LARGEST_PACK_SIZE);
  std::vector<std::byte> payload = encodeMessage(acceptance);
  acceptance.content.size = ONE_BYTE_TOO_LARGE;
  payload[LAST_SIZE_BYTE] = payload[LAST_SIZE_BYTE] | LOWEST_BIT;

  EXPECT_THROW((void)encodeMessage(acceptance), InvalidFieldValueException);
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(payload),
               InvalidFieldValueException);
}

/**
 * Given the payload of an acceptance cut short at each of its sizes
 * When each is decoded
 * Then each is refused as cut short
 */
TEST(ConnectionAccepted, RefusesAPayloadCutAtEverySize) {
  const std::vector<std::byte> payload = payloadOnTheProtocolPage();

  for (std::size_t size = 0; size < payload.size(); ++size) {
    SCOPED_TRACE(size);
    EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(
                     std::span{payload.data(), size}),
                 BufferUnderflowException);
  }
}

/**
 * Given the payload of an acceptance followed by one more byte
 * When it is decoded
 * Then it is refused for that byte
 */
TEST(ConnectionAccepted, RefusesAByteAfterThePort) {
  std::vector<std::byte> payload = payloadOnTheProtocolPage();
  payload.push_back(TRAILING_BYTE);

  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(payload),
               TrailingBytesException);
}

}  // namespace rtype::network::testing

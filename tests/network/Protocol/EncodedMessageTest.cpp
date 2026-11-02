#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "EncodedMessage.hpp"
#include "MessageType.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network {

using testing::ENCODED_FILLING_MESSAGE_SIZE;
using testing::FILLING_PAYLOAD_SIZE;

/**
 * Given a message of FILLING_PAYLOAD_SIZE bytes of payload
 * When its size in a datagram is asked
 * Then it is its type, its size and its payload: 589 bytes
 */
TEST(EncodedMessage, CountsThePrefixAndThePayloadOfAMessage) {
  EXPECT_EQ(encodedSize(EncodedMessage{
                .type = MessageType::GameEvent,
                .payload = std::vector<std::byte>(FILLING_PAYLOAD_SIZE)}),
            ENCODED_FILLING_MESSAGE_SIZE);
}

}  // namespace rtype::network

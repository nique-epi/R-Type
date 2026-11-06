#include <gtest/gtest.h>
#include "ByteSequence.hpp"
#include "GameStatusAcknowledged.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"

namespace rtype::network::testing {

/**
 * Given an acknowledgement
 * When it is encoded
 * Then the payload is empty
 */
TEST(GameStatusAcknowledged, HasNoPayload) {
  EXPECT_TRUE(encodeMessage(GameStatusAcknowledged{}).empty());
}

/**
 * Given an acknowledgement
 * When it is encoded then decoded
 * Then the decoded acknowledgement equals the original
 */
TEST(GameStatusAcknowledged, DecodesWhatItEncoded) {
  EXPECT_EQ(decodeMessage<GameStatusAcknowledged>(
                encodeMessage(GameStatusAcknowledged{})),
            GameStatusAcknowledged{});
}

/**
 * Given a payload of one byte
 * When it is decoded
 * Then it is refused, since the message has no payload
 */
TEST(GameStatusAcknowledged, RefusesAByteOfPayload) {
  EXPECT_THROW(static_cast<void>(
                   decodeMessage<GameStatusAcknowledged>(byteSequence({0x00}))),
               TrailingBytesException);
}

}  // namespace rtype::network::testing

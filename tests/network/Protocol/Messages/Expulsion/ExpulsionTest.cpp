#include <gtest/gtest.h>
#include "ByteSequence.hpp"
#include "Expulsion.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"

namespace rtype::network::testing {

/**
 * Given an expulsion by the host
 * When it is encoded
 * Then the payload is the reason 1 of the protocol page
 */
TEST(Expulsion, EncodesTheReason) {
  EXPECT_EQ(encodeMessage(Expulsion{.reason = ExpulsionReason::ByTheHost}),
            byteSequence({0x01}));
}

/**
 * Given the payloads 01 and 02
 * When each is decoded
 * Then they give an expulsion by the host and a ban
 */
TEST(Expulsion, DecodesBothReasonsOfTheProtocolPage) {
  EXPECT_EQ(decodeMessage<Expulsion>(byteSequence({0x01})).reason,
            ExpulsionReason::ByTheHost);
  EXPECT_EQ(decodeMessage<Expulsion>(byteSequence({0x02})).reason,
            ExpulsionReason::Banned);
}

/**
 * Given the reason 0, just below the first of the protocol page
 * When it is decoded
 * Then it is refused
 */
TEST(Expulsion, RefusesTheReasonBelowTheFirst) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<Expulsion>(byteSequence({0x00}))),
      InvalidFieldValueException);
}

/**
 * Given the reason 3, just above the last of the protocol page
 * When it is decoded
 * Then it is refused
 */
TEST(Expulsion, RefusesTheReasonAboveTheLast) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<Expulsion>(byteSequence({0x03}))),
      InvalidFieldValueException);
}

/**
 * Given an empty payload
 * When it is decoded
 * Then it is refused for being cut short
 */
TEST(Expulsion, RefusesAnEmptyPayload) {
  EXPECT_THROW(static_cast<void>(decodeMessage<Expulsion>(byteSequence({}))),
               BufferUnderflowException);
}

}  // namespace rtype::network::testing

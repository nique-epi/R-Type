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
 * Given the payload 01
 * When it is decoded
 * Then it is an expulsion by the host
 */
TEST(Expulsion, DecodesTheReasonOfTheProtocolPage) {
  EXPECT_EQ(decodeMessage<Expulsion>(byteSequence({0x01})).reason,
            ExpulsionReason::ByTheHost);
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
 * Given the reason 2, just above the last of the protocol page
 * When it is decoded
 * Then it is refused
 */
TEST(Expulsion, RefusesTheReasonAboveTheLast) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<Expulsion>(byteSequence({0x02}))),
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

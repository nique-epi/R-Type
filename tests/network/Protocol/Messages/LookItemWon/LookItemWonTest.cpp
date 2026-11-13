#include <gtest/gtest.h>
#include "ByteSequence.hpp"
#include "LookItemWon.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"

namespace rtype::network::testing {

/**
 * Given the item 3, the crown
 * When it is encoded
 * Then the payload is one byte: 03
 */
TEST(LookItemWon, EncodesTheItemInOneByte) {
  EXPECT_EQ(encodeMessage(LookItemWon{.item = 3}), byteSequence({0x03}));
}

/**
 * Given the last accessory of the catalog
 * When it is encoded then decoded
 * Then the decoded message equals the original
 */
TEST(LookItemWon, DecodesWhatItEncoded) {
  const LookItemWon won{.item = 25};

  EXPECT_EQ(decodeMessage<LookItemWon>(encodeMessage(won)), won);
}

/**
 * Given a payload whose item is 0, which is no item
 * When it is decoded
 * Then it is refused
 */
TEST(LookItemWon, RefusesNoItem) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<LookItemWon>(byteSequence({0x00}))),
      InvalidFieldValueException);
}

/**
 * Given a payload whose item is 31, one more than the catalog holds
 * When it is decoded
 * Then it is refused
 */
TEST(LookItemWon, RefusesANumberAboveTheCatalog) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<LookItemWon>(byteSequence({0x1F}))),
      InvalidFieldValueException);
}

/**
 * Given a payload whose item is 26, the first hull
 * When it is decoded
 * Then it is refused: a hull is never won in a game
 */
TEST(LookItemWon, RefusesAHull) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<LookItemWon>(byteSequence({0x1A}))),
      InvalidFieldValueException);
}

/**
 * Given an empty payload
 * When it is decoded
 * Then it is refused
 */
TEST(LookItemWon, RefusesAnEmptyPayload) {
  EXPECT_THROW(static_cast<void>(decodeMessage<LookItemWon>(byteSequence({}))),
               BufferUnderflowException);
}

/**
 * Given a payload of two bytes
 * When it is decoded
 * Then it is refused
 */
TEST(LookItemWon, RefusesASecondByte) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<LookItemWon>(byteSequence({0x03, 0x00}))),
      TrailingBytesException);
}

}  // namespace rtype::network::testing

#include <gtest/gtest.h>
#include "ByteSequence.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "ShipLookChoice.hpp"

namespace rtype::network::testing {

/**
 * Given the look head 2, eyes 9, face 10
 * When it is encoded
 * Then the payload is the three bytes in that order: 02 09 0A
 */
TEST(ShipLookChoice, EncodesHeadEyesFaceInThreeBytes) {
  EXPECT_EQ(
      encodeMessage(ShipLookChoice{.look = {.head = 2, .eyes = 9, .face = 10}}),
      byteSequence({0x02, 0x09, 0x0A}));
}

/**
 * Given a look that wears an item in every slot
 * When it is encoded then decoded
 * Then the decoded choice equals the original
 */
TEST(ShipLookChoice, DecodesWhatItEncoded) {
  const ShipLookChoice choice{.look = {.head = 8, .eyes = 9, .face = 10}};

  EXPECT_EQ(decodeMessage<ShipLookChoice>(encodeMessage(choice)), choice);
}

/**
 * Given a payload that puts the shades, item 9, on the head
 * When it is decoded
 * Then it is refused
 */
TEST(ShipLookChoice, RefusesAnItemOfAnotherSlot) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ShipLookChoice>(
                   byteSequence({0x09, 0x00, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a payload whose head is 11, one more than the catalog holds
 * When it is decoded
 * Then it is refused
 */
TEST(ShipLookChoice, RefusesANumberAboveTheCatalog) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ShipLookChoice>(
                   byteSequence({0x0B, 0x00, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a payload of two bytes
 * When it is decoded
 * Then it is refused
 */
TEST(ShipLookChoice, RefusesTwoBytes) {
  EXPECT_THROW(static_cast<void>(
                   decodeMessage<ShipLookChoice>(byteSequence({0x01, 0x00}))),
               BufferUnderflowException);
}

/**
 * Given a payload of four bytes
 * When it is decoded
 * Then it is refused
 */
TEST(ShipLookChoice, RefusesAFourthByte) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ShipLookChoice>(
                   byteSequence({0x01, 0x00, 0x00, 0x00}))),
               TrailingBytesException);
}

}  // namespace rtype::network::testing

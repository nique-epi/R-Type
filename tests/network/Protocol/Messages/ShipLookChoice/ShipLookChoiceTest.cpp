#include <gtest/gtest.h>
#include "ByteSequence.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "ShipLookChoice.hpp"

namespace rtype::network::testing {

/**
 * Given the look head 2, eyes 9, face 10, hull 26
 * When it is encoded
 * Then the payload is the four bytes in that order: 02 09 0A 1A
 */
TEST(ShipLookChoice, EncodesHeadEyesFaceHullInFourBytes) {
  EXPECT_EQ(encodeMessage(ShipLookChoice{
                .look = {.head = 2, .eyes = 9, .face = 10, .hull = 26}}),
            byteSequence({0x02, 0x09, 0x0A, 0x1A}));
}

/**
 * Given a look that wears an item in every slot
 * When it is encoded then decoded
 * Then the decoded choice equals the original
 */
TEST(ShipLookChoice, DecodesWhatItEncoded) {
  const ShipLookChoice choice{
      .look = {.head = 22, .eyes = 24, .face = 25, .hull = 30}};

  EXPECT_EQ(decodeMessage<ShipLookChoice>(encodeMessage(choice)), choice);
}

/**
 * Given a payload that puts the shades, item 9, on the head
 * When it is decoded
 * Then it is refused
 */
TEST(ShipLookChoice, RefusesAnItemOfAnotherSlot) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ShipLookChoice>(
                   byteSequence({0x09, 0x00, 0x00, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a payload whose hull byte is 1, the top hat
 * When it is decoded
 * Then it is refused: only a hull flies in the hull slot
 */
TEST(ShipLookChoice, RefusesAnAccessoryAsAHull) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ShipLookChoice>(
                   byteSequence({0x00, 0x00, 0x00, 0x01}))),
               InvalidFieldValueException);
}

/**
 * Given a payload whose head byte is 26, the first hull
 * When it is decoded
 * Then it is refused: a hull is worn in no other slot
 */
TEST(ShipLookChoice, RefusesAHullOnTheHead) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ShipLookChoice>(
                   byteSequence({0x1A, 0x00, 0x00, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a payload whose head is 31, one more than the catalog holds
 * When it is decoded
 * Then it is refused
 */
TEST(ShipLookChoice, RefusesANumberAboveTheCatalog) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ShipLookChoice>(
                   byteSequence({0x1F, 0x00, 0x00, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a payload of three bytes
 * When it is decoded
 * Then it is refused
 */
TEST(ShipLookChoice, RefusesThreeBytes) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ShipLookChoice>(
                   byteSequence({0x01, 0x00, 0x00}))),
               BufferUnderflowException);
}

/**
 * Given a payload of five bytes
 * When it is decoded
 * Then it is refused
 */
TEST(ShipLookChoice, RefusesAFifthByte) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ShipLookChoice>(
                   byteSequence({0x01, 0x00, 0x00, 0x00, 0x00}))),
               TrailingBytesException);
}

}  // namespace rtype::network::testing

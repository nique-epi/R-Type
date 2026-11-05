#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ByteSequence.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "OrderedMessage.hpp"
#include "ProtocolConstants.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network {

using testing::byteSequence;
using testing::CARRIED_PAYLOAD_LIMIT;
using testing::CARRIED_TYPE;
using testing::SOME_FIRST_KEPT;
using testing::SOME_ORDERED_NUMBER;

/**
 * Given an OrderedMessage carrying two bytes
 * When it is encoded
 * Then the payload is the number, firstKept, the carried type, then the bytes
 */
TEST(OrderedMessage, IsWrittenAsItsFieldsThenTheCarriedBytes) {
  const OrderedMessage message{.number = SOME_ORDERED_NUMBER,
                               .firstKept = SOME_FIRST_KEPT,
                               .innerType = CARRIED_TYPE,
                               .payload = byteSequence({0xAA, 0xBB})};

  EXPECT_EQ(encodeMessage(message),
            byteSequence({0x12, 0x34, 0x12, 0x30, 0x0D, 0xAA, 0xBB}));
}

/**
 * Given an OrderedMessage
 * When it is encoded then decoded
 * Then the same message comes back
 */
TEST(OrderedMessage, ComesBackAsItWasEncoded) {
  const OrderedMessage message{.number = SOME_ORDERED_NUMBER,
                               .firstKept = SOME_FIRST_KEPT,
                               .innerType = CARRIED_TYPE,
                               .payload = byteSequence({0x01, 0x02, 0x03})};

  EXPECT_EQ(decodeMessage<OrderedMessage>(encodeMessage(message)), message);
}

/**
 * Given an OrderedMessage carrying nothing
 * When it is encoded then decoded
 * Then the same message comes back
 */
TEST(OrderedMessage, CarriesAnEmptyPayload) {
  const OrderedMessage message{.number = SOME_ORDERED_NUMBER,
                               .firstKept = SOME_ORDERED_NUMBER,
                               .innerType = CARRIED_TYPE,
                               .payload = {}};

  EXPECT_EQ(decodeMessage<OrderedMessage>(encodeMessage(message)), message);
}

/**
 * Given an OrderedMessage carrying an OrderedMessage
 * When it is encoded
 * Then it is refused
 */
TEST(OrderedMessage, RefusesToCarryAnOrderedMessage) {
  const OrderedMessage message{
      .number = SOME_ORDERED_NUMBER,
      .firstKept = SOME_FIRST_KEPT,
      .innerType = static_cast<std::uint8_t>(MessageType::OrderedMessage),
      .payload = {}};

  EXPECT_THROW((void)encodeMessage(message), InvalidFieldValueException);
}

/**
 * Given an OrderedMessage whose firstKept is later than its number
 * When it is encoded
 * Then it is refused
 */
TEST(OrderedMessage, RefusesAFirstKeptLaterThanItsNumber) {
  const OrderedMessage message{.number = SOME_FIRST_KEPT,
                               .firstKept = SOME_ORDERED_NUMBER,
                               .innerType = CARRIED_TYPE,
                               .payload = {}};

  EXPECT_THROW((void)encodeMessage(message), InvalidFieldValueException);
}

/**
 * Given an OrderedMessage carrying the largest payload a datagram holds
 * When it is encoded
 * Then it is accepted
 */
TEST(OrderedMessage, AcceptsTheLargestCarriedPayload) {
  const OrderedMessage message{
      .number = SOME_ORDERED_NUMBER,
      .firstKept = SOME_FIRST_KEPT,
      .innerType = CARRIED_TYPE,
      .payload = std::vector<std::byte>(CARRIED_PAYLOAD_LIMIT)};

  EXPECT_NO_THROW((void)encodeMessage(message));
}

/**
 * Given an OrderedMessage carrying one byte more than a datagram holds
 * When it is encoded
 * Then it is refused
 */
TEST(OrderedMessage, RefusesOneByteMoreThanTheLargestCarriedPayload) {
  const OrderedMessage message{
      .number = SOME_ORDERED_NUMBER,
      .firstKept = SOME_FIRST_KEPT,
      .innerType = CARRIED_TYPE,
      .payload = std::vector<std::byte>(CARRIED_PAYLOAD_LIMIT + 1)};

  EXPECT_THROW((void)encodeMessage(message), InvalidFieldValueException);
}

/**
 * Given a payload one byte short of the five bytes before the carried bytes
 * When it is decoded
 * Then it is refused as cut short
 */
TEST(OrderedMessage, RefusesAPayloadCutShortBeforeTheCarriedType) {
  const std::vector<std::byte> payload = byteSequence({0x00, 0x09, 0x00, 0x05});

  EXPECT_THROW((void)decodeMessage<OrderedMessage>(payload),
               BufferUnderflowException);
}

}  // namespace rtype::network

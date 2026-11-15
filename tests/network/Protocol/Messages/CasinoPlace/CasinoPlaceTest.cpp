#include <gtest/gtest.h>
#include "ByteSequence.hpp"
#include "CasinoPlace.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"

namespace rtype::network::testing {

/**
 * Given a player who sits at the roulette table
 * When the CasinoPlace is encoded
 * Then the payload is one byte: 01
 */
TEST(CasinoPlace, EncodesAtTableAsOne) {
  EXPECT_EQ(encodeMessage(CasinoPlace{.place = TablePlace::AtTable}),
            byteSequence({0x01}));
}

/**
 * Given a player who goes back to the room
 * When the CasinoPlace is encoded then decoded
 * Then the decoded place is the room
 */
TEST(CasinoPlace, DecodesWhatItEncoded) {
  const CasinoPlace back{.place = TablePlace::InRoom};

  EXPECT_EQ(decodeMessage<CasinoPlace>(encodeMessage(back)), back);
}

/**
 * Given a payload whose place is 02, which no place has
 * When it is decoded
 * Then it is refused
 */
TEST(CasinoPlace, RefusesAnUnknownPlace) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<CasinoPlace>(byteSequence({0x02}))),
      InvalidFieldValueException);
}

/**
 * Given an empty payload
 * When it is decoded
 * Then it is refused
 */
TEST(CasinoPlace, RefusesAnEmptyPayload) {
  EXPECT_THROW(static_cast<void>(decodeMessage<CasinoPlace>(byteSequence({}))),
               BufferUnderflowException);
}

/**
 * Given a payload of two bytes
 * When it is decoded
 * Then it is refused
 */
TEST(CasinoPlace, RefusesASecondByte) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<CasinoPlace>(byteSequence({0x01, 0x00}))),
      TrailingBytesException);
}

}  // namespace rtype::network::testing

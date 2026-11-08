#include <gtest/gtest.h>
#include <string>
#include "ByteSequence.hpp"
#include "ExpulsionOrder.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given the order to expel ACE
 * When it is encoded
 * Then the payload is the nickname after its length: 03 41 43 45
 */
TEST(ExpulsionOrder, EncodesTheNicknameAfterItsLength) {
  EXPECT_EQ(
      encodeMessage(ExpulsionOrder{.nickname = std::string{ACE_IN_CAPITALS}}),
      byteSequence({0x03, 0x41, 0x43, 0x45}));
}

/**
 * Given an order whose nickname holds sixteen bytes, the longest
 * When it is encoded then decoded
 * Then the decoded order equals the original
 */
TEST(ExpulsionOrder, DecodesWhatItEncodedAtTheLongestNickname) {
  const ExpulsionOrder longest{.nickname = std::string{SIXTEEN_BYTE_NICKNAME}};

  EXPECT_EQ(decodeMessage<ExpulsionOrder>(encodeMessage(longest)), longest);
}

/**
 * Given an order whose nickname holds seventeen bytes
 * When it is encoded
 * Then the nickname is refused
 */
TEST(ExpulsionOrder, RefusesANicknameTooLong) {
  EXPECT_THROW(static_cast<void>(encodeMessage(ExpulsionOrder{
                   .nickname = std::string{SEVENTEEN_BYTE_NICKNAME}})),
               InvalidNicknameException);
}

/**
 * Given the payload of an order to expel ACE, followed by one more byte
 * When it is decoded
 * Then the byte after the nickname is refused
 */
TEST(ExpulsionOrder, RefusesABytePastTheNickname) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ExpulsionOrder>(
                   byteSequence({0x03, 0x41, 0x43, 0x45, 0x00}))),
               TrailingBytesException);
}

/**
 * Given the payload of an order whose nickname is empty
 * When it is decoded
 * Then the nickname is refused
 */
TEST(ExpulsionOrder, RefusesAnEmptyNickname) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<ExpulsionOrder>(byteSequence({0x00}))),
      InvalidNicknameException);
}

}  // namespace rtype::network::testing

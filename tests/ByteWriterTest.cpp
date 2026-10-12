#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "ByteSequence.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "SerializationConstants.hpp"

namespace rtype::network {
namespace {

using testing::byteSequence;

constexpr std::uint32_t SAMPLE_UINT32 = 0x01020304;
constexpr std::uint16_t SAMPLE_UINT16 = 0xABCD;
constexpr std::uint64_t SAMPLE_UINT64 = 0x0102030405060708;
constexpr std::int16_t SAMPLE_NEGATIVE_INT16 = -2;
constexpr float SAMPLE_FLOAT_ONE = 1.0F;
constexpr std::size_t OVERSIZED_STRING_LENGTH = MAX_SHORT_STRING_LENGTH + 1;
constexpr std::uint8_t EARLIER_BYTE = 0x07;
constexpr std::uint16_t SECOND_VALUE = 0x0203;

/**
 * Given an empty writer
 * When a 32-bit integer is written
 * Then the most significant byte comes first
 */
TEST(ByteWriter, WritesUint32MostSignificantByteFirst) {
  ByteWriter writer;

  writer.writeUint32(SAMPLE_UINT32);

  EXPECT_EQ(writer.bytes(), byteSequence({0x01, 0x02, 0x03, 0x04}));
}

/**
 * Given an empty writer
 * When a 16-bit integer is written
 * Then the most significant byte comes first
 */
TEST(ByteWriter, WritesUint16MostSignificantByteFirst) {
  ByteWriter writer;

  writer.writeUint16(SAMPLE_UINT16);

  EXPECT_EQ(writer.bytes(), byteSequence({0xAB, 0xCD}));
}

/**
 * Given an empty writer
 * When a 64-bit integer is written
 * Then the most significant byte comes first
 */
TEST(ByteWriter, WritesUint64MostSignificantByteFirst) {
  ByteWriter writer;

  writer.writeUint64(SAMPLE_UINT64);

  EXPECT_EQ(writer.bytes(),
            byteSequence({0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}));
}

/**
 * Given an empty writer
 * When the 16-bit integer -2 is written
 * Then it is encoded in two's complement
 */
TEST(ByteWriter, WritesNegativeInt16InTwosComplement) {
  ByteWriter writer;

  writer.writeInt16(SAMPLE_NEGATIVE_INT16);

  EXPECT_EQ(writer.bytes(), byteSequence({0xFF, 0xFE}));
}

/**
 * Given an empty writer
 * When the float 1.0 is written
 * Then it is encoded as IEEE-754 binary32, most significant byte first
 */
TEST(ByteWriter, WritesFloatAsIeeeBinary32) {
  ByteWriter writer;

  writer.writeFloat(SAMPLE_FLOAT_ONE);

  EXPECT_EQ(writer.bytes(), byteSequence({0x3F, 0x80, 0x00, 0x00}));
}

/**
 * Given an empty writer
 * When the string "Hi" is written
 * Then a one-byte length precedes the raw characters
 */
TEST(ByteWriter, WritesStringWithOneByteLengthPrefix) {
  ByteWriter writer;

  writer.writeString("Hi");

  EXPECT_EQ(writer.bytes(), byteSequence({0x02, 0x48, 0x69}));
}

/**
 * Given an empty writer
 * When a string of the maximum length is written
 * Then the prefix announces that length and every character follows
 */
TEST(ByteWriter, AcceptsStringOfMaximumLength) {
  ByteWriter writer;

  writer.writeString(std::string(MAX_SHORT_STRING_LENGTH, 'a'));

  EXPECT_EQ(writer.bytes().size(),
            SHORT_STRING_PREFIX_SIZE + MAX_SHORT_STRING_LENGTH);
  EXPECT_EQ(writer.bytes().front(), static_cast<std::byte>(0xFF));
}

/**
 * Given an empty writer
 * When a string one byte longer than the maximum is written
 * Then a StringTooLongException is raised
 */
TEST(ByteWriter, RejectsStringLongerThanLengthPrefixCanAnnounce) {
  ByteWriter writer;

  EXPECT_THROW(writer.writeString(std::string(OVERSIZED_STRING_LENGTH, 'a')),
               StringTooLongException);
}

/**
 * Given a writer that already holds one value
 * When a too long string is rejected
 * Then the buffer still holds only the earlier value
 */
TEST(ByteWriter, RejectedStringLeavesBufferUntouched) {
  ByteWriter writer;
  writer.writeUint8(EARLIER_BYTE);

  EXPECT_THROW(writer.writeString(std::string(OVERSIZED_STRING_LENGTH, 'a')),
               StringTooLongException);

  EXPECT_EQ(writer.bytes(), byteSequence({0x07}));
}

/**
 * Given a writer
 * When values of several types are written one after the other
 * Then the buffer holds them back to back in call order
 */
TEST(ByteWriter, AppendsValuesInCallOrder) {
  ByteWriter writer;

  writer.writeUint8(0x01);
  writer.writeUint16(SECOND_VALUE);
  writer.writeString("A");

  EXPECT_EQ(writer.bytes(), byteSequence({0x01, 0x02, 0x03, 0x01, 0x41}));
}

/**
 * Given a StringTooLongException
 * When it is caught as the module's root exception
 * Then the catch succeeds
 */
TEST(ByteWriter, StringErrorIsCaughtAsNetworkException) {
  ByteWriter writer;

  EXPECT_THROW(writer.writeString(std::string(OVERSIZED_STRING_LENGTH, 'a')),
               NetworkException);
}

}  // namespace
}  // namespace rtype::network

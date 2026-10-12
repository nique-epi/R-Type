#include <gtest/gtest.h>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "SerializationConstants.hpp"

namespace rtype::network {
namespace {

constexpr std::uint32_t QUIET_NAN_BITS = 0x7FC00001;
constexpr std::uint8_t SAMPLE_UINT8 = 7;
constexpr std::int32_t SAMPLE_INT32 = -100000;
constexpr float SAMPLE_FLOAT = 3.5F;
constexpr std::uint64_t SAMPLE_UINT64 = 0x1122334455667788;
constexpr std::uint64_t TRUNCATED_UINT64 = 0x0102030405060708;

/**
 * Given the smallest, a middle and the largest unsigned 8-bit values
 * When each is written then read back
 * Then every value is unchanged
 */
TEST(SerializationRoundTrip, Uint8) {
  for (const std::uint8_t value : {std::uint8_t{0}, std::uint8_t{1},
                                   std::numeric_limits<std::uint8_t>::max()}) {
    ByteWriter writer;
    writer.writeUint8(value);
    ByteReader reader(writer.bytes());

    EXPECT_EQ(reader.readUint8(), value);
  }
}

/**
 * Given the smallest, a middle and the largest unsigned 16-bit values
 * When each is written then read back
 * Then every value is unchanged
 */
TEST(SerializationRoundTrip, Uint16) {
  for (const std::uint16_t value :
       {std::uint16_t{0}, std::uint16_t{1}, std::uint16_t{0x0102},
        std::numeric_limits<std::uint16_t>::max()}) {
    ByteWriter writer;
    writer.writeUint16(value);
    ByteReader reader(writer.bytes());

    EXPECT_EQ(reader.readUint16(), value);
  }
}

/**
 * Given the smallest, a middle and the largest unsigned 32-bit values
 * When each is written then read back
 * Then every value is unchanged
 */
TEST(SerializationRoundTrip, Uint32) {
  for (const std::uint32_t value :
       {std::uint32_t{0}, std::uint32_t{1}, std::uint32_t{0x01020304},
        std::numeric_limits<std::uint32_t>::max()}) {
    ByteWriter writer;
    writer.writeUint32(value);
    ByteReader reader(writer.bytes());

    EXPECT_EQ(reader.readUint32(), value);
  }
}

/**
 * Given the smallest, a middle and the largest unsigned 64-bit values
 * When each is written then read back
 * Then every value is unchanged
 */
TEST(SerializationRoundTrip, Uint64) {
  for (const std::uint64_t value :
       {std::uint64_t{0}, std::uint64_t{1}, std::uint64_t{0x0102030405060708},
        std::numeric_limits<std::uint64_t>::max()}) {
    ByteWriter writer;
    writer.writeUint64(value);
    ByteReader reader(writer.bytes());

    EXPECT_EQ(reader.readUint64(), value);
  }
}

/**
 * Given the extremes, -1 and 0 of the signed 8-bit range
 * When each is written then read back
 * Then every value is unchanged
 */
TEST(SerializationRoundTrip, Int8) {
  for (const std::int8_t value :
       {std::numeric_limits<std::int8_t>::min(), std::int8_t{-1},
        std::int8_t{0}, std::numeric_limits<std::int8_t>::max()}) {
    ByteWriter writer;
    writer.writeInt8(value);
    ByteReader reader(writer.bytes());

    EXPECT_EQ(reader.readInt8(), value);
  }
}

/**
 * Given the extremes, -1 and 0 of the signed 16-bit range
 * When each is written then read back
 * Then every value is unchanged
 */
TEST(SerializationRoundTrip, Int16) {
  for (const std::int16_t value :
       {std::numeric_limits<std::int16_t>::min(), std::int16_t{-1},
        std::int16_t{0}, std::numeric_limits<std::int16_t>::max()}) {
    ByteWriter writer;
    writer.writeInt16(value);
    ByteReader reader(writer.bytes());

    EXPECT_EQ(reader.readInt16(), value);
  }
}

/**
 * Given the extremes, -1 and 0 of the signed 32-bit range
 * When each is written then read back
 * Then every value is unchanged
 */
TEST(SerializationRoundTrip, Int32) {
  for (const std::int32_t value :
       {std::numeric_limits<std::int32_t>::min(), std::int32_t{-1},
        std::int32_t{0}, std::numeric_limits<std::int32_t>::max()}) {
    ByteWriter writer;
    writer.writeInt32(value);
    ByteReader reader(writer.bytes());

    EXPECT_EQ(reader.readInt32(), value);
  }
}

/**
 * Given the extremes, -1 and 0 of the signed 64-bit range
 * When each is written then read back
 * Then every value is unchanged
 */
TEST(SerializationRoundTrip, Int64) {
  for (const std::int64_t value :
       {std::numeric_limits<std::int64_t>::min(), std::int64_t{-1},
        std::int64_t{0}, std::numeric_limits<std::int64_t>::max()}) {
    ByteWriter writer;
    writer.writeInt64(value);
    ByteReader reader(writer.bytes());

    EXPECT_EQ(reader.readInt64(), value);
  }
}

/**
 * Given zero, negative zero, ordinary values, the extremes and infinity
 * When each float is written then read back
 * Then the bit pattern is unchanged
 */
TEST(SerializationRoundTrip, FloatKeepsEveryBit) {
  for (const float value :
       {0.0F, -0.0F, 1.5F, -2.25F, std::numeric_limits<float>::max(),
        std::numeric_limits<float>::denorm_min(),
        std::numeric_limits<float>::infinity()}) {
    ByteWriter writer;
    writer.writeFloat(value);
    ByteReader reader(writer.bytes());

    EXPECT_EQ(std::bit_cast<std::uint32_t>(reader.readFloat()),
              std::bit_cast<std::uint32_t>(value));
  }
}

/**
 * Given a NaN with a specific payload
 * When it is written then read back
 * Then the bit pattern is unchanged
 */
TEST(SerializationRoundTrip, NanKeepsItsPayload) {
  const auto value = std::bit_cast<float>(QUIET_NAN_BITS);
  ByteWriter writer;
  writer.writeFloat(value);
  ByteReader reader(writer.bytes());

  EXPECT_EQ(std::bit_cast<std::uint32_t>(reader.readFloat()), QUIET_NAN_BITS);
}

/**
 * Given an empty string, a short one, one of the maximum length and one with
 * an embedded zero byte
 * When each is written then read back
 * Then every string is unchanged
 */
TEST(SerializationRoundTrip, String) {
  const std::string embeddedZero("a\0b", 3);
  for (const std::string& value :
       {std::string(), std::string("Hello"),
        std::string(MAX_SHORT_STRING_LENGTH, 'z'), embeddedZero}) {
    ByteWriter writer;
    writer.writeString(value);
    ByteReader reader(writer.bytes());

    EXPECT_EQ(reader.readString(), value);
    EXPECT_EQ(reader.remaining(), 0U);
  }
}

/**
 * Given values of every type written one after the other
 * When they are read back in the same order
 * Then each equals what was written and no byte remains
 */
TEST(SerializationRoundTrip, MixedSequence) {
  ByteWriter writer;
  writer.writeUint8(SAMPLE_UINT8);
  writer.writeInt32(SAMPLE_INT32);
  writer.writeString("player");
  writer.writeFloat(SAMPLE_FLOAT);
  writer.writeUint64(SAMPLE_UINT64);
  ByteReader reader(writer.bytes());

  EXPECT_EQ(reader.readUint8(), SAMPLE_UINT8);
  EXPECT_EQ(reader.readInt32(), SAMPLE_INT32);
  EXPECT_EQ(reader.readString(), "player");
  EXPECT_EQ(reader.readFloat(), SAMPLE_FLOAT);
  EXPECT_EQ(reader.readUint64(), SAMPLE_UINT64);
  EXPECT_EQ(reader.remaining(), 0U);
}

/**
 * Given the encoding of a 64-bit integer
 * When the buffer is cut at every length shorter than the encoding
 * Then reading the integer always raises a BufferUnderflowException
 */
TEST(SerializationRoundTrip, EveryTruncationOfUint64IsRejected) {
  ByteWriter writer;
  writer.writeUint64(TRUNCATED_UINT64);
  const std::vector<std::byte> full = writer.bytes();

  for (std::size_t length = 0; length < full.size(); ++length) {
    const std::vector<std::byte> truncated(
        full.begin(), full.begin() + static_cast<std::ptrdiff_t>(length));
    ByteReader reader(truncated);

    EXPECT_THROW(reader.readUint64(), BufferUnderflowException)
        << "length " << length;
  }
}

/**
 * Given the encoding of a string
 * When the buffer is cut at every length shorter than the encoding
 * Then reading the string always raises a BufferUnderflowException
 */
TEST(SerializationRoundTrip, EveryTruncationOfStringIsRejected) {
  ByteWriter writer;
  writer.writeString("Hello");
  const std::vector<std::byte> full = writer.bytes();

  for (std::size_t length = 0; length < full.size(); ++length) {
    const std::vector<std::byte> truncated(
        full.begin(), full.begin() + static_cast<std::ptrdiff_t>(length));
    ByteReader reader(truncated);

    EXPECT_THROW(reader.readString(), BufferUnderflowException)
        << "length " << length;
  }
}

}  // namespace
}  // namespace rtype::network

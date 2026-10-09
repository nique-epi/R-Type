#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ByteReader.hpp"
#include "ByteSequence.hpp"
#include "NetworkException.hpp"

namespace rtype::network {
namespace {

using testing::byteSequence;

constexpr std::uint32_t EXPECTED_UINT32 = 0x01020304;
constexpr std::uint16_t EXPECTED_UINT16 = 0xABCD;
constexpr std::int16_t EXPECTED_NEGATIVE_INT16 = -2;
constexpr float EXPECTED_FLOAT_ONE = 1.0F;

/**
 * Given the bytes 01 02 03 04
 * When a 32-bit integer is read
 * Then the first byte is the most significant
 */
TEST(ByteReader, ReadsUint32MostSignificantByteFirst) {
  const auto buffer = byteSequence({0x01, 0x02, 0x03, 0x04});
  ByteReader reader(buffer);

  EXPECT_EQ(reader.readUint32(), EXPECTED_UINT32);
}

/**
 * Given the bytes AB CD
 * When a 16-bit integer is read
 * Then the first byte is the most significant
 */
TEST(ByteReader, ReadsUint16MostSignificantByteFirst) {
  const auto buffer = byteSequence({0xAB, 0xCD});
  ByteReader reader(buffer);

  EXPECT_EQ(reader.readUint16(), EXPECTED_UINT16);
}

/**
 * Given the bytes FF FE
 * When a signed 16-bit integer is read
 * Then the value is -2
 */
TEST(ByteReader, ReadsNegativeInt16FromTwosComplement) {
  const auto buffer = byteSequence({0xFF, 0xFE});
  ByteReader reader(buffer);

  EXPECT_EQ(reader.readInt16(), EXPECTED_NEGATIVE_INT16);
}

/**
 * Given the IEEE-754 bytes of 1.0
 * When a float is read
 * Then the value is 1.0
 */
TEST(ByteReader, ReadsFloatFromIeeeBinary32) {
  const auto buffer = byteSequence({0x3F, 0x80, 0x00, 0x00});
  ByteReader reader(buffer);

  EXPECT_EQ(reader.readFloat(), EXPECTED_FLOAT_ONE);
}

/**
 * Given the bytes 02 48 69
 * When a string is read
 * Then the value is "Hi" and nothing remains
 */
TEST(ByteReader, ReadsStringFromOneByteLengthPrefix) {
  const auto buffer = byteSequence({0x02, 0x48, 0x69});
  ByteReader reader(buffer);

  EXPECT_EQ(reader.readString(), "Hi");
  EXPECT_EQ(reader.remaining(), 0U);
}

/**
 * Given a buffer holding three bytes
 * When two of them are read
 * Then one byte remains
 */
TEST(ByteReader, RemainingShrinksByTheBytesRead) {
  const auto buffer = byteSequence({0x01, 0x02, 0x03});
  ByteReader reader(buffer);

  reader.readUint16();

  EXPECT_EQ(reader.remaining(), 1U);
}

/**
 * Given an empty buffer
 * When a one-byte integer is read
 * Then a BufferUnderflowException is raised
 */
TEST(ByteReader, RejectsReadFromEmptyBuffer) {
  const std::vector<std::byte> buffer;
  ByteReader reader(buffer);

  EXPECT_THROW(reader.readUint8(), BufferUnderflowException);
}

/**
 * Given an empty buffer
 * When a string is read
 * Then a BufferUnderflowException is raised
 */
TEST(ByteReader, RejectsStringReadFromEmptyBuffer) {
  const std::vector<std::byte> buffer;
  ByteReader reader(buffer);

  EXPECT_THROW(reader.readString(), BufferUnderflowException);
}

/**
 * Given a buffer one byte shorter than a 16-bit integer
 * When a 16-bit integer is read
 * Then a BufferUnderflowException is raised
 */
TEST(ByteReader, RejectsTruncatedUint16) {
  const auto buffer = byteSequence({0x01});
  ByteReader reader(buffer);

  EXPECT_THROW(reader.readUint16(), BufferUnderflowException);
}

/**
 * Given a buffer one byte shorter than a 32-bit integer
 * When a 32-bit integer is read
 * Then a BufferUnderflowException is raised
 */
TEST(ByteReader, RejectsTruncatedUint32) {
  const auto buffer = byteSequence({0x01, 0x02, 0x03});
  ByteReader reader(buffer);

  EXPECT_THROW(reader.readUint32(), BufferUnderflowException);
}

/**
 * Given a buffer one byte shorter than a 64-bit integer
 * When a 64-bit integer is read
 * Then a BufferUnderflowException is raised
 */
TEST(ByteReader, RejectsTruncatedUint64) {
  const auto buffer = byteSequence({1, 2, 3, 4, 5, 6, 7});
  ByteReader reader(buffer);

  EXPECT_THROW(reader.readUint64(), BufferUnderflowException);
}

/**
 * Given a buffer one byte shorter than a float
 * When a float is read
 * Then a BufferUnderflowException is raised
 */
TEST(ByteReader, RejectsTruncatedFloat) {
  const auto buffer = byteSequence({0x3F, 0x80, 0x00});
  ByteReader reader(buffer);

  EXPECT_THROW(reader.readFloat(), BufferUnderflowException);
}

/**
 * Given a string prefix announcing 200 bytes followed by only 3
 * When the string is read
 * Then a BufferUnderflowException is raised
 */
TEST(ByteReader, RejectsStringLongerThanTheBytesThatFollow) {
  const auto buffer = byteSequence({200, 0x41, 0x42, 0x43});
  ByteReader reader(buffer);

  EXPECT_THROW(reader.readString(), BufferUnderflowException);
}

/**
 * Given a string prefix announcing 200 bytes followed by only 3
 * When the string read is rejected
 * Then the position has not moved
 */
TEST(ByteReader, RejectedStringReadDoesNotMovePosition) {
  const auto buffer = byteSequence({200, 0x41, 0x42, 0x43});
  ByteReader reader(buffer);

  EXPECT_THROW(reader.readString(), BufferUnderflowException);

  EXPECT_EQ(reader.remaining(), buffer.size());
}

/**
 * Given a buffer of three bytes
 * When a 32-bit read is rejected
 * Then a one-byte read afterwards still returns the first byte
 */
TEST(ByteReader, RejectedReadDoesNotConsumeBytes) {
  const auto buffer = byteSequence({0x09, 0x08, 0x07});
  ByteReader reader(buffer);

  EXPECT_THROW(reader.readUint32(), BufferUnderflowException);

  EXPECT_EQ(reader.readUint8(), 0x09);
  EXPECT_EQ(reader.remaining(), 2U);
}

/**
 * Given a buffer fully consumed by earlier reads
 * When one more byte is read
 * Then a BufferUnderflowException is raised
 */
TEST(ByteReader, RejectsReadAfterTheEnd) {
  const auto buffer = byteSequence({0x01});
  ByteReader reader(buffer);
  reader.readUint8();

  EXPECT_THROW(reader.readUint8(), BufferUnderflowException);
}

/**
 * Given a BufferUnderflowException
 * When it is caught as the module's root exception
 * Then the catch succeeds
 */
TEST(ByteReader, UnderflowIsCaughtAsNetworkException) {
  const std::vector<std::byte> buffer;
  ByteReader reader(buffer);

  EXPECT_THROW(reader.readUint8(), NetworkException);
}

}  // namespace
}  // namespace rtype::network

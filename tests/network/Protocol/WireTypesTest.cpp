#include <gtest/gtest.h>
#include <limits>
#include <string_view>
#include "ByteReader.hpp"
#include "ByteSequence.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolTestConstants.hpp"
#include "WireTypes.hpp"
#include "WorldStateFixtures.hpp"

namespace rtype::network {
namespace {

using testing::ACCENTED_LETTER;
using testing::BEYOND_RANGE_X;
using testing::BEYOND_RANGE_Y;
using testing::byteSequence;
using testing::EXAMPLE_Y;
using testing::FOUR_ROCKETS;
using testing::ILL_FORMED_UTF8;
using testing::LARGEST_WIRE_COORDINATE;
using testing::MINUS_ONE_UNIT;
using testing::NEAREST_EIGHTH_OF_UNROUNDED_X;
using testing::ONE_BYTE_NICKNAME;
using testing::SEVENTEEN_BYTE_NICKNAME;
using testing::SIXTEEN_BYTE_NICKNAME;
using testing::SMALLEST_WIRE_COORDINATE;
using testing::UNROUNDED_X;
using testing::WELL_FORMED_UTF8_AT_THE_EDGES;

/**
 * Given the value true
 * When it is written
 * Then the wire holds the byte 01
 */
TEST(WireTypes, WritesTrueAsOne) {
  ByteWriter writer;

  writeBool(writer, true);

  EXPECT_EQ(writer.bytes(), byteSequence({0x01}));
}

/**
 * Given the value false
 * When it is written
 * Then the wire holds the byte 00
 */
TEST(WireTypes, WritesFalseAsZero) {
  ByteWriter writer;

  writeBool(writer, false);

  EXPECT_EQ(writer.bytes(), byteSequence({0x00}));
}

/**
 * Given the byte 01
 * When a bool is read
 * Then it is true
 */
TEST(WireTypes, ReadsOneAsTrue) {
  const auto buffer = byteSequence({0x01});
  ByteReader reader(buffer);

  EXPECT_TRUE(readBool(reader));
}

/**
 * Given the byte 00
 * When a bool is read
 * Then it is false
 */
TEST(WireTypes, ReadsZeroAsFalse) {
  const auto buffer = byteSequence({0x00});
  ByteReader reader(buffer);

  EXPECT_FALSE(readBool(reader));
}

/**
 * Given the byte 02, the first after 1, and the byte FF, the last a byte holds
 * When each is read as a bool
 * Then each is refused, since a bool on the wire is 0 or 1
 */
TEST(WireTypes, RejectsABoolOtherThanZeroOrOne) {
  const auto two = byteSequence({0x02});
  const auto largest = byteSequence({0xFF});
  ByteReader twoReader(two);
  ByteReader largestReader(largest);

  EXPECT_THROW((void)readBool(twoReader), InvalidBoolException);
  EXPECT_THROW((void)readBool(largestReader), InvalidBoolException);
}

/**
 * Given the coordinate 50.5 units
 * When it is written
 * Then the wire holds 404 eighths, most significant byte first: 01 94
 */
TEST(WireTypes, WritesACoordinateInEighthsOfAUnit) {
  ByteWriter writer;

  writeCoordinate(writer, EXAMPLE_Y);

  EXPECT_EQ(writer.bytes(), byteSequence({0x01, 0x94}));
}

/**
 * Given the coordinate -1 unit
 * When it is written
 * Then the wire holds -8 eighths in two's complement: FF F8
 */
TEST(WireTypes, WritesANegativeCoordinateInTwosComplement) {
  ByteWriter writer;

  writeCoordinate(writer, MINUS_ONE_UNIT);

  EXPECT_EQ(writer.bytes(), byteSequence({0xFF, 0xF8}));
}

/**
 * Given the bytes 01 94
 * When a coordinate is read
 * Then it is 404 eighths, that is 50.5 units
 */
TEST(WireTypes, ReadsEighthsBackAsUnits) {
  const auto buffer = byteSequence({0x01, 0x94});
  ByteReader reader(buffer);

  EXPECT_EQ(readCoordinate(reader), EXAMPLE_Y);
}

/**
 * Given a coordinate that is not a multiple of an eighth of a unit
 * When it is written then read
 * Then it comes back as the nearest eighth
 */
TEST(WireTypes, RoundsACoordinateToTheNearestEighth) {
  ByteWriter writer;
  writeCoordinate(writer, UNROUNDED_X);
  ByteReader reader(writer.bytes());

  EXPECT_EQ(readCoordinate(reader), NEAREST_EIGHTH_OF_UNROUNDED_X);
}

/**
 * Given a coordinate above 4095.875 units
 * When it is written then read
 * Then it comes back as 4095.875, the largest the wire holds
 */
TEST(WireTypes, ClampsACoordinateAboveTheRange) {
  ByteWriter writer;
  writeCoordinate(writer, BEYOND_RANGE_X);
  ByteReader reader(writer.bytes());

  EXPECT_EQ(readCoordinate(reader), LARGEST_WIRE_COORDINATE);
}

/**
 * Given a coordinate below -4096 units
 * When it is written then read
 * Then it comes back as -4096, the smallest the wire holds
 */
TEST(WireTypes, ClampsACoordinateBelowTheRange) {
  ByteWriter writer;
  writeCoordinate(writer, BEYOND_RANGE_Y);
  ByteReader reader(writer.bytes());

  EXPECT_EQ(readCoordinate(reader), SMALLEST_WIRE_COORDINATE);
}

/**
 * Given a coordinate that is not a number
 * When it is written
 * Then an InvalidCoordinateException is raised
 */
TEST(WireTypes, RefusesACoordinateThatIsNotANumber) {
  ByteWriter writer;

  EXPECT_THROW(writeCoordinate(writer, std::numeric_limits<float>::quiet_NaN()),
               InvalidCoordinateException);
}

/**
 * Given a coordinate that is not a number
 * When writing it is refused
 * Then nothing was added to the buffer
 */
TEST(WireTypes, RefusedCoordinateWritesNothing) {
  ByteWriter writer;

  EXPECT_THROW(writeCoordinate(writer, std::numeric_limits<float>::quiet_NaN()),
               InvalidCoordinateException);

  EXPECT_TRUE(writer.bytes().empty());
}

/**
 * Given characters at every edge of the table of RFC 3629
 * When each is checked
 * Then each is well-formed UTF-8
 */
TEST(WireTypes, AcceptsUtf8AtEveryEdgeOfTheTable) {
  for (const std::string_view text : WELL_FORMED_UTF8_AT_THE_EDGES) {
    SCOPED_TRACE(::testing::PrintToString(text));
    EXPECT_TRUE(isValidUtf8(text));
  }
}

/**
 * Given overlong forms, a surrogate, a code point above U+10FFFF, a byte no
 * sequence starts with, a stray continuation byte and a cut sequence
 * When each is checked
 * Then none is well-formed UTF-8
 */
TEST(WireTypes, RejectsIllFormedUtf8) {
  for (const std::string_view text : ILL_FORMED_UTF8) {
    SCOPED_TRACE(::testing::PrintToString(text));
    EXPECT_FALSE(isValidUtf8(text));
  }
}

/**
 * Given an empty text
 * When it is checked
 * Then it is well-formed UTF-8
 */
TEST(WireTypes, AcceptsAnEmptyTextAsUtf8) { EXPECT_TRUE(isValidUtf8({})); }

/**
 * Given nicknames of 1 and 16 bytes
 * When each is checked
 * Then each is accepted
 */
TEST(WireTypes, AcceptsNicknamesOfOneToSixteenBytes) {
  EXPECT_TRUE(isValidNickname(ONE_BYTE_NICKNAME));
  EXPECT_TRUE(isValidNickname(SIXTEEN_BYTE_NICKNAME));
}

/**
 * Given four characters of four bytes each, sixteen bytes in all
 * When the nickname is checked
 * Then it is accepted, since the limit counts bytes
 */
TEST(WireTypes, AcceptsSixteenBytesOfFourByteCharacters) {
  EXPECT_TRUE(isValidNickname(FOUR_ROCKETS));
}

/**
 * Given an accented letter, two bytes of UTF-8
 * When the nickname is checked
 * Then it is accepted
 */
TEST(WireTypes, AcceptsAnAccentedNickname) {
  EXPECT_TRUE(isValidNickname(ACCENTED_LETTER));
}

/**
 * Given an empty nickname
 * When it is checked
 * Then it is refused
 */
TEST(WireTypes, RejectsAnEmptyNickname) { EXPECT_FALSE(isValidNickname({})); }

/**
 * Given a nickname of 17 bytes
 * When it is checked
 * Then it is refused
 */
TEST(WireTypes, RejectsANicknameOfSeventeenBytes) {
  EXPECT_FALSE(isValidNickname(SEVENTEEN_BYTE_NICKNAME));
}

/**
 * Given a nickname of a size the protocol accepts that is not UTF-8
 * When it is checked
 * Then it is refused
 */
TEST(WireTypes, RejectsANicknameThatIsNotUtf8) {
  EXPECT_FALSE(isValidNickname(ILL_FORMED_UTF8.front()));
}

}  // namespace
}  // namespace rtype::network

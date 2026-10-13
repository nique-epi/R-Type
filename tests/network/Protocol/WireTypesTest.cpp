#include <gtest/gtest.h>
#include "ByteReader.hpp"
#include "ByteSequence.hpp"
#include "ByteWriter.hpp"
#include "ProtocolTestConstants.hpp"
#include "WireTypes.hpp"
#include "WorldStateFixtures.hpp"

namespace rtype::network {
namespace {

using testing::BEYOND_RANGE_X;
using testing::BEYOND_RANGE_Y;
using testing::byteSequence;
using testing::EXAMPLE_Y;
using testing::LARGEST_WIRE_COORDINATE;
using testing::MINUS_ONE_UNIT;
using testing::NEAREST_EIGHTH_OF_UNROUNDED_X;
using testing::SMALLEST_WIRE_COORDINATE;
using testing::UNROUNDED_X;

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

}  // namespace
}  // namespace rtype::network

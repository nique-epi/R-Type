#include "WireTypes.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

void writeBool(ByteWriter& writer, bool value) {
  writer.writeUint8(value ? WIRE_TRUE : WIRE_FALSE);
}

bool readBool(ByteReader& reader) {
  const std::uint8_t value = reader.readUint8();
  if (value != WIRE_FALSE && value != WIRE_TRUE) {
    throw InvalidBoolException(value);
  }
  return value == WIRE_TRUE;
}

void writeCoordinate(ByteWriter& writer, float coordinate) {
  if (std::isnan(coordinate)) {
    throw InvalidCoordinateException();
  }
  const float steps = std::round(coordinate * WIRE_COORDINATE_STEPS_PER_UNIT);
  writer.writeInt16(static_cast<std::int16_t>(
      std::clamp(steps, MINIMUM_WIRE_STEPS, MAXIMUM_WIRE_STEPS)));
}

float readCoordinate(ByteReader& reader) {
  return static_cast<float>(reader.readInt16()) /
         WIRE_COORDINATE_STEPS_PER_UNIT;
}

}  // namespace rtype::network

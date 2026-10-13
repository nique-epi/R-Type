#include "WireTypes.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

void writeCoordinate(ByteWriter& writer, float coordinate) {
  const float steps = std::round(coordinate * WIRE_COORDINATE_STEPS_PER_UNIT);
  writer.writeInt16(static_cast<std::int16_t>(
      std::clamp(steps, MINIMUM_WIRE_STEPS, MAXIMUM_WIRE_STEPS)));
}

float readCoordinate(ByteReader& reader) {
  return static_cast<float>(reader.readInt16()) /
         WIRE_COORDINATE_STEPS_PER_UNIT;
}

}  // namespace rtype::network

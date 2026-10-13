#pragma once

#include "ByteReader.hpp"
#include "ByteWriter.hpp"

namespace rtype::network {

/**
 * @brief Writes a coordinate in logical units the way the protocol sends it: a
 * signed 16-bit count of eighths of a unit, rounded to the nearest eighth and
 * clamped to -4096 to 4095.875 units.
 */
void writeCoordinate(ByteWriter& writer, float coordinate);

/**
 * @brief Reads a coordinate written by writeCoordinate(), in logical units.
 * @throws BufferUnderflowException when fewer than two bytes remain.
 */
float readCoordinate(ByteReader& reader);

}  // namespace rtype::network

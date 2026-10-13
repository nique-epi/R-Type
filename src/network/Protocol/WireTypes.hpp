#pragma once

#include "ByteReader.hpp"
#include "ByteWriter.hpp"

namespace rtype::network {

/**
 * @brief Writes @p value as one byte, WIRE_TRUE or WIRE_FALSE.
 */
void writeBool(ByteWriter& writer, bool value);

/**
 * @brief Reads a bool written by writeBool().
 * @throws InvalidBoolException when the byte is neither WIRE_FALSE nor
 * WIRE_TRUE.
 * @throws BufferUnderflowException when no byte remains.
 */
bool readBool(ByteReader& reader);

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

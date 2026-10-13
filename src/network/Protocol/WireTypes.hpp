#pragma once

#include <cstdint>
#include <string_view>
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
 * @throws InvalidCoordinateException when @p coordinate is NaN; nothing is
 * written then.
 */
void writeCoordinate(ByteWriter& writer, float coordinate);

/**
 * @brief Reads a coordinate written by writeCoordinate(), in logical units.
 * @throws BufferUnderflowException when fewer than two bytes remain.
 */
float readCoordinate(ByteReader& reader);

/**
 * @returns Whether @p text is well-formed UTF-8 as RFC 3629 defines it: no
 * overlong form, no surrogate, nothing above U+10FFFF, no stray continuation
 * byte and no sequence cut short. An empty text is well-formed.
 */
[[nodiscard]] bool isValidUtf8(std::string_view text);

/**
 * @returns Whether @p nickname is a nickname the protocol accepts:
 * MINIMUM_NICKNAME_SIZE to MAXIMUM_NICKNAME_SIZE bytes of well-formed UTF-8.
 */
[[nodiscard]] bool isValidNickname(std::string_view nickname);

/**
 * @brief Checks that @p nickname is one the protocol accepts, see
 * isValidNickname().
 * @throws InvalidNicknameException when it is not.
 */
void requireValidNickname(std::string_view nickname);

/**
 * @brief Checks that @p playerNumber is the number of a player of a game,
 * FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS.
 * @throws InvalidFieldValueException naming playerNumber when it is not.
 */
void requireValidPlayerNumber(std::uint8_t playerNumber);

/**
 * @brief Checks that a payload was read to its end.
 * @throws TrailingBytesException when bytes remain in @p reader.
 */
void requireFullyRead(const ByteReader& reader);

}  // namespace rtype::network

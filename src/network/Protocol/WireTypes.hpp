#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string_view>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief Writes @p value as one byte, TRUE_WIRE_VALUE or FALSE_WIRE_VALUE.
 */
void writeBool(ByteWriter& writer, bool value);

/**
 * @brief Reads a bool written by writeBool().
 * @throws InvalidBoolException when the byte is neither FALSE_WIRE_VALUE nor
 * TRUE_WIRE_VALUE.
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

/**
 * @brief Reads the number of players a message announces, and checks it
 * before anything is sized from it.
 * @throws TooManyPlayersException when it is above MAXIMUM_PLAYERS.
 * @throws BufferUnderflowException when no byte remains.
 */
[[nodiscard]] std::uint8_t readPlayerCount(ByteReader& reader);

/**
 * @brief Checks the players a message lists: at most MAXIMUM_PLAYERS, each
 * with a player number of a game, and no number twice. Player is a struct with
 * a playerNumber field.
 * @throws TooManyPlayersException when there are more than MAXIMUM_PLAYERS.
 * @throws InvalidFieldValueException when a number is not FIRST_PLAYER_NUMBER
 * to MAXIMUM_PLAYERS.
 * @throws DuplicatePlayerNumberException when a number appears twice.
 */
template <typename Player>
void requireValidPlayers(std::span<const Player> players) {
  if (players.size() > MAXIMUM_PLAYERS) {
    throw TooManyPlayersException(players.size(), MAXIMUM_PLAYERS);
  }
  std::array<bool, MAXIMUM_PLAYERS + 1> listed{};
  for (const Player& player : players) {
    requireValidPlayerNumber(player.playerNumber);
    if (listed.at(player.playerNumber)) {
      throw DuplicatePlayerNumberException(player.playerNumber);
    }
    listed.at(player.playerNumber) = true;
  }
}

}  // namespace rtype::network

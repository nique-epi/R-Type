#include "ServerInformation.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

/**
 * @brief Checks that the game version and the name follow their rules.
 * @throws InvalidServerTextException naming the first that does not.
 */
void requireValidTexts(const ServerInformation& message) {
  if (!isValidGameVersion(message.gameVersion)) {
    throw InvalidServerTextException(GAME_VERSION_FIELD,
                                     message.gameVersion.size());
  }
  if (!isValidServerName(message.name)) {
    throw InvalidServerTextException(SERVER_NAME_FIELD, message.name.size());
  }
}

/**
 * @brief Checks that the players, then the players and the free seats
 * together, fit in the seats of the games.
 * @throws InvalidFieldValueException naming the first count that does not.
 */
void requireValidCounts(const ServerInformation& message) {
  const std::uint32_t seats =
      static_cast<std::uint32_t>(message.gameCount) * MAXIMUM_PLAYERS;
  if (message.playerCount > seats) {
    throw InvalidFieldValueException(PLAYER_COUNT_FIELD, message.playerCount);
  }
  if (message.freeSeatCount > seats - message.playerCount) {
    throw InvalidFieldValueException(FREE_SEAT_COUNT_FIELD,
                                     message.freeSeatCount);
  }
}

void requireValid(const ServerInformation& message) {
  requireValidTexts(message);
  requireValidCounts(message);
}

}  // namespace

void encodePayload(ByteWriter& writer, const ServerInformation& message) {
  requireValid(message);
  writer.writeUint32(message.sentAt);
  writer.writeUint32(message.heldFor);
  writer.writeUint8(message.protocolVersion);
  writer.writeString(message.gameVersion);
  writer.writeString(message.name);
  writer.writeUint16(message.playerCount);
  writer.writeUint16(message.gameCount);
  writer.writeUint16(message.freeSeatCount);
}

ServerInformation decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<ServerInformation> type) {
  ByteReader reader{payload};
  ServerInformation message;
  message.sentAt = reader.readUint32();
  message.heldFor = reader.readUint32();
  message.protocolVersion = reader.readUint8();
  message.gameVersion = reader.readString();
  message.name = reader.readString();
  message.playerCount = reader.readUint16();
  message.gameCount = reader.readUint16();
  message.freeSeatCount = reader.readUint16();
  requireFullyRead(reader);
  requireValid(message);
  return message;
}

}  // namespace rtype::network

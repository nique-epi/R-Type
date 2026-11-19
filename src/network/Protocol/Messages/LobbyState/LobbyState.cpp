#include "LobbyState.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include <vector>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

/** @throws InvalidFieldValueException naming mapCount when @p count is past
 * MAXIMUM_LOBBY_MAPS. */
void requireMapCount(std::size_t count) {
  if (count > MAXIMUM_LOBBY_MAPS) {
    throw InvalidFieldValueException(MAP_COUNT_FIELD, count);
  }
}

/** @brief Checks the maps of @p message, and the vote of each player against
 * them; see encodePayload(). */
void validateMaps(const LobbyState& message) {
  requireMapCount(message.maps.size());
  for (const std::string& map : message.maps) {
    if (!isValidNickname(map)) {
      throw InvalidFieldValueException(MAP_NAME_FIELD, map.size());
    }
  }
  for (const LobbyPlayer& player : message.players) {
    if (player.mapVote > message.maps.size()) {
      throw InvalidFieldValueException(MAP_VOTE_FIELD, player.mapVote);
    }
  }
}

void validate(const LobbyState& message) {
  requireValidPlayers(std::span<const LobbyPlayer>{message.players});
  for (const LobbyPlayer& player : message.players) {
    requireValidNickname(player.nickname);
  }
  validateMaps(message);
}

LobbyPlayer readPlayer(ByteReader& reader) {
  return LobbyPlayer{.playerNumber = reader.readUint8(),
                     .ready = readBool(reader),
                     .nickname = reader.readString(),
                     .mapVote = reader.readUint8()};
}

/** @returns The names of the maps @p reader holds, their number checked
 * before any is read. */
std::vector<std::string> readMaps(ByteReader& reader) {
  const std::uint8_t count = reader.readUint8();
  requireMapCount(count);
  std::vector<std::string> maps;
  maps.reserve(count);
  for (std::uint8_t index = 0; index < count; ++index) {
    maps.push_back(reader.readString());
  }
  return maps;
}

}  // namespace

void encodePayload(ByteWriter& writer, const LobbyState& message) {
  validate(message);
  writer.writeUint8(static_cast<std::uint8_t>(message.players.size()));
  for (const LobbyPlayer& player : message.players) {
    writer.writeUint8(player.playerNumber);
    writeBool(writer, player.ready);
    writer.writeString(player.nickname);
    writer.writeUint8(player.mapVote);
  }
  writer.writeUint8(static_cast<std::uint8_t>(message.maps.size()));
  for (const std::string& map : message.maps) {
    writer.writeString(map);
  }
}

LobbyState decodePayload(std::span<const std::byte> payload,
                         [[maybe_unused]] std::type_identity<LobbyState> type) {
  ByteReader reader{payload};
  const std::uint8_t playerCount = readPlayerCount(reader);
  LobbyState message;
  message.players.reserve(playerCount);
  for (std::uint8_t index = 0; index < playerCount; ++index) {
    message.players.push_back(readPlayer(reader));
  }
  message.maps = readMaps(reader);
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network

#include "LobbyState.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

void validate(const LobbyState& message) {
  requireValidPlayers(std::span<const LobbyPlayer>{message.players});
  for (const LobbyPlayer& player : message.players) {
    requireValidNickname(player.nickname);
  }
}

LobbyPlayer readPlayer(ByteReader& reader) {
  return LobbyPlayer{.playerNumber = reader.readUint8(),
                     .ready = readBool(reader),
                     .nickname = reader.readString()};
}

}  // namespace

void encodePayload(ByteWriter& writer, const LobbyState& message) {
  validate(message);
  writer.writeUint8(static_cast<std::uint8_t>(message.players.size()));
  for (const LobbyPlayer& player : message.players) {
    writer.writeUint8(player.playerNumber);
    writeBool(writer, player.ready);
    writer.writeString(player.nickname);
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
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network

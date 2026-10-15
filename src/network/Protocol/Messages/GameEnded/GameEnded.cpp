#include "GameEnded.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload(ByteWriter& writer, const GameEnded& message) {
  requireValidPlayers(std::span<const PlayerScore>{message.players});
  writer.writeUint8(static_cast<std::uint8_t>(message.players.size()));
  for (const PlayerScore& player : message.players) {
    writer.writeUint8(player.playerNumber);
    writer.writeUint32(player.score);
  }
}

GameEnded decodePayload(std::span<const std::byte> payload,
                        [[maybe_unused]] std::type_identity<GameEnded> type) {
  ByteReader reader{payload};
  const std::uint8_t playerCount = readPlayerCount(reader);
  GameEnded message;
  message.players.reserve(playerCount);
  for (std::uint8_t index = 0; index < playerCount; ++index) {
    message.players.push_back(PlayerScore{.playerNumber = reader.readUint8(),
                                          .score = reader.readUint32()});
  }
  requireFullyRead(reader);
  requireValidPlayers(std::span<const PlayerScore>{message.players});
  return message;
}

}  // namespace rtype::network

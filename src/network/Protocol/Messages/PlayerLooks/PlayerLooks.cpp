#include "PlayerLooks.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

void validate(const PlayerLooks& message) {
  requireValidPlayers(std::span<const PlayerLook>{message.players});
}

PlayerLook readPlayer(ByteReader& reader) {
  const std::uint8_t playerNumber = reader.readUint8();
  return PlayerLook{.playerNumber = playerNumber, .look = readShipLook(reader)};
}

}  // namespace

const identifiers::ShipLook* findLook(std::span<const PlayerLook> looks,
                                      std::uint8_t number) {
  const auto found =
      std::ranges::find(looks, number, &PlayerLook::playerNumber);
  return found == looks.end() ? nullptr : &found->look;
}

void encodePayload(ByteWriter& writer, const PlayerLooks& message) {
  validate(message);
  writer.writeUint8(static_cast<std::uint8_t>(message.players.size()));
  for (const PlayerLook& player : message.players) {
    writer.writeUint8(player.playerNumber);
    writeShipLook(writer, player.look);
  }
}

PlayerLooks decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<PlayerLooks> type) {
  ByteReader reader{payload};
  const std::uint8_t playerCount = readPlayerCount(reader);
  PlayerLooks message;
  message.players.reserve(playerCount);
  for (std::uint8_t index = 0; index < playerCount; ++index) {
    message.players.push_back(readPlayer(reader));
  }
  requireFullyRead(reader);
  validate(message);
  return message;
}

}  // namespace rtype::network

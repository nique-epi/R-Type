#include "RoundTrips.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

PlayerRoundTrip readPlayer(ByteReader& reader) {
  return PlayerRoundTrip{.playerNumber = reader.readUint8(),
                         .roundTrip = reader.readUint32(),
                         .jitter = reader.readUint32()};
}

}  // namespace

void encodePayload(ByteWriter& writer, const RoundTrips& message) {
  requireValidPlayers(std::span<const PlayerRoundTrip>{message.players});
  writer.writeUint8(static_cast<std::uint8_t>(message.players.size()));
  for (const PlayerRoundTrip& player : message.players) {
    writer.writeUint8(player.playerNumber);
    writer.writeUint32(player.roundTrip);
    writer.writeUint32(player.jitter);
  }
}

RoundTrips decodePayload(std::span<const std::byte> payload,
                         [[maybe_unused]] std::type_identity<RoundTrips> type) {
  ByteReader reader{payload};
  const std::uint8_t playerCount = readPlayerCount(reader);
  RoundTrips message;
  message.players.reserve(playerCount);
  for (std::uint8_t index = 0; index < playerCount; ++index) {
    message.players.push_back(readPlayer(reader));
  }
  requireFullyRead(reader);
  requireValidPlayers(std::span<const PlayerRoundTrip>{message.players});
  return message;
}

}  // namespace rtype::network

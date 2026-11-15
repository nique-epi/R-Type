#include "LobbyState.hpp"
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
 * @throws InvalidFieldValueException when @p place is not one of PilotPlace.
 */
void requireListedPlace(PilotPlace place) {
  switch (place) {
    case PilotPlace::NotReady:
    case PilotPlace::Ready:
    case PilotPlace::AtCasino:
      return;
  }
  throw InvalidFieldValueException(PLACE_FIELD,
                                   static_cast<std::uint8_t>(place));
}

void validate(const LobbyState& message) {
  requireValidPlayers(std::span<const LobbyPlayer>{message.players});
  for (const LobbyPlayer& player : message.players) {
    requireListedPlace(player.place);
    requireValidNickname(player.nickname);
  }
}

LobbyPlayer readPlayer(ByteReader& reader) {
  const std::uint8_t playerNumber = reader.readUint8();
  const auto place = static_cast<PilotPlace>(reader.readUint8());
  return LobbyPlayer{.playerNumber = playerNumber,
                     .place = place,
                     .nickname = reader.readString()};
}

}  // namespace

void encodePayload(ByteWriter& writer, const LobbyState& message) {
  validate(message);
  writer.writeUint8(static_cast<std::uint8_t>(message.players.size()));
  for (const LobbyPlayer& player : message.players) {
    writer.writeUint8(player.playerNumber);
    writer.writeUint8(static_cast<std::uint8_t>(player.place));
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

#include "GameStatus.hpp"
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

bool isListed(GameStatusState state) {
  switch (state) {
    case GameStatusState::WaitingForPlayers:
    case GameStatusState::InProgress:
    case GameStatusState::Finished:
      return true;
  }
  return false;
}

void validate(const GameStatus& message) {
  if (!isListed(message.state)) {
    throw InvalidFieldValueException(STATE_FIELD,
                                     static_cast<std::uint8_t>(message.state));
  }
  if (message.playerCount > MAXIMUM_PLAYERS) {
    throw InvalidFieldValueException(PLAYER_COUNT_FIELD, message.playerCount);
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const GameStatus& message) {
  validate(message);
  writer.writeUint8(static_cast<std::uint8_t>(message.state));
  writer.writeUint8(message.playerCount);
}

GameStatus decodePayload(std::span<const std::byte> payload,
                         [[maybe_unused]] std::type_identity<GameStatus> type) {
  ByteReader reader{payload};
  const auto state = static_cast<GameStatusState>(reader.readUint8());
  const std::uint8_t playerCount = reader.readUint8();
  requireFullyRead(reader);
  const GameStatus message{.state = state, .playerCount = playerCount};
  validate(message);
  return message;
}

}  // namespace rtype::network

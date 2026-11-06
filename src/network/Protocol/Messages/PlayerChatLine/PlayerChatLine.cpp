#include "PlayerChatLine.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "ChatLine.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload(ByteWriter& writer, const PlayerChatLine& message) {
  requireValidPlayerNumber(message.playerNumber);
  encodePayload(writer, message.line);
  writer.writeUint8(message.playerNumber);
}

PlayerChatLine decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<PlayerChatLine> type) {
  ByteReader reader{payload};
  PlayerChatLine message;
  message.line = readChatLine(reader);
  message.playerNumber = reader.readUint8();
  requireFullyRead(reader);
  requireValidPlayerNumber(message.playerNumber);
  return message;
}

}  // namespace rtype::network

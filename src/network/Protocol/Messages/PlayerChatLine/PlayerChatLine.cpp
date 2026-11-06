#include "PlayerChatLine.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "ChatLine.hpp"
#include "NetworkException.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

void requireValid(const PlayerChatLine& message) {
  if (!isValidChatText(message.line.text)) {
    throw InvalidChatTextException(message.line.text.size());
  }
  requireValidPlayerNumber(message.playerNumber);
}

}  // namespace

void encodePayload(ByteWriter& writer, const PlayerChatLine& message) {
  requireValid(message);
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
  requireValid(message);
  return message;
}

}  // namespace rtype::network

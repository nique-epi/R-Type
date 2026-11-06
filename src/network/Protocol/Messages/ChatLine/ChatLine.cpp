#include "ChatLine.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

void requireValid(const ChatLine& message) {
  if (!isValidChatText(message.text)) {
    throw InvalidChatTextException(message.text.size());
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const ChatLine& message) {
  requireValid(message);
  writer.writeUint16(message.lineNumber);
  writer.writeString(message.text);
}

ChatLine readChatLine(ByteReader& reader) {
  ChatLine message;
  message.lineNumber = reader.readUint16();
  message.text = reader.readString();
  requireValid(message);
  return message;
}

ChatLine decodePayload(std::span<const std::byte> payload,
                       [[maybe_unused]] std::type_identity<ChatLine> type) {
  ByteReader reader{payload};
  ChatLine message = readChatLine(reader);
  requireFullyRead(reader);
  return message;
}

}  // namespace rtype::network

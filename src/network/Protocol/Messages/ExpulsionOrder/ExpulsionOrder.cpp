#include "ExpulsionOrder.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload(ByteWriter& writer, const ExpulsionOrder& message) {
  requireValidNickname(message.nickname);
  writer.writeString(message.nickname);
}

ExpulsionOrder decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<ExpulsionOrder> type) {
  ByteReader reader{payload};
  ExpulsionOrder message{.nickname = reader.readString()};
  requireFullyRead(reader);
  requireValidNickname(message.nickname);
  return message;
}

}  // namespace rtype::network

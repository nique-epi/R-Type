#include "LookItemWon.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload(ByteWriter& writer, const LookItemWon& message) {
  requireValidLookItem(message.item);
  writer.writeUint8(message.item);
}

LookItemWon decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<LookItemWon> type) {
  ByteReader reader{payload};
  const LookItemWon message{.item = reader.readUint8()};
  requireValidLookItem(message.item);
  requireFullyRead(reader);
  return message;
}

}  // namespace rtype::network

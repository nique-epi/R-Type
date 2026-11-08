#include "ShipLookChoice.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload(ByteWriter& writer, const ShipLookChoice& message) {
  writeShipLook(writer, message.look);
}

ShipLookChoice decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<ShipLookChoice> type) {
  ByteReader reader{payload};
  const ShipLookChoice message{.look = readShipLook(reader)};
  requireFullyRead(reader);
  return message;
}

}  // namespace rtype::network

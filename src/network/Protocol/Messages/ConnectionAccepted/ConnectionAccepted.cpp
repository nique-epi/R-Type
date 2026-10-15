#include "ConnectionAccepted.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload(ByteWriter& writer, const ConnectionAccepted& message) {
  requireValidPlayerNumber(message.playerNumber);
  writer.writeUint8(message.playerNumber);
}

ConnectionAccepted decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<ConnectionAccepted> type) {
  ByteReader reader{payload};
  const ConnectionAccepted message{.playerNumber = reader.readUint8()};
  requireFullyRead(reader);
  requireValidPlayerNumber(message.playerNumber);
  return message;
}

}  // namespace rtype::network

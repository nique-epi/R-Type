#include "ConnectionChallenge.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload(ByteWriter& writer, const ConnectionChallenge& message) {
  writer.writeUint64(message.cookie);
}

ConnectionChallenge decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<ConnectionChallenge> type) {
  ByteReader reader{payload};
  const ConnectionChallenge message{.cookie = reader.readUint64()};
  requireFullyRead(reader);
  return message;
}

}  // namespace rtype::network

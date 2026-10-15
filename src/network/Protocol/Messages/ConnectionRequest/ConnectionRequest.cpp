#include "ConnectionRequest.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload(ByteWriter& writer, const ConnectionRequest& message) {
  requireValidNickname(message.nickname);
  writer.writeUint64(message.cookie);
  writer.writeString(message.nickname);
}

ConnectionRequest decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<ConnectionRequest> type) {
  ByteReader reader{payload};
  ConnectionRequest message{.cookie = reader.readUint64(),
                            .nickname = reader.readString()};
  requireFullyRead(reader);
  requireValidNickname(message.nickname);
  return message;
}

}  // namespace rtype::network

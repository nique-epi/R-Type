#include "MasterProbe.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload(ByteWriter& writer, const MasterProbe& message) {
  writer.writeUint64(message.secret);
  writer.writeUint64(message.token);
}

MasterProbe decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<MasterProbe> type) {
  ByteReader reader{payload};
  const MasterProbe message{.secret = reader.readUint64(),
                            .token = reader.readUint64()};
  requireFullyRead(reader);
  return message;
}

}  // namespace rtype::network

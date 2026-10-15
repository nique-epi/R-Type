#include "ReadyState.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload(ByteWriter& writer, const ReadyState& message) {
  writeBool(writer, message.ready);
}

ReadyState decodePayload(std::span<const std::byte> payload,
                         [[maybe_unused]] std::type_identity<ReadyState> type) {
  ByteReader reader{payload};
  const ReadyState message{.ready = readBool(reader)};
  requireFullyRead(reader);
  return message;
}

}  // namespace rtype::network

#include "Heartbeat.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload([[maybe_unused]] ByteWriter& writer,
                   [[maybe_unused]] const Heartbeat& message) {}

Heartbeat decodePayload(std::span<const std::byte> payload,
                        [[maybe_unused]] std::type_identity<Heartbeat> type) {
  const ByteReader reader{payload};
  requireFullyRead(reader);
  return Heartbeat{};
}

}  // namespace rtype::network

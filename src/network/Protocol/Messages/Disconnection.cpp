#include "Disconnection.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload([[maybe_unused]] ByteWriter& writer,
                   [[maybe_unused]] const Disconnection& message) {}

Disconnection decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<Disconnection> type) {
  const ByteReader reader{payload};
  requireFullyRead(reader);
  return Disconnection{};
}

}  // namespace rtype::network

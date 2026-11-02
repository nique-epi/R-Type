#include "GameLoaded.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload([[maybe_unused]] ByteWriter& writer,
                   [[maybe_unused]] const GameLoaded& message) {}

GameLoaded decodePayload(std::span<const std::byte> payload,
                         [[maybe_unused]] std::type_identity<GameLoaded> type) {
  const ByteReader reader{payload};
  requireFullyRead(reader);
  return GameLoaded{};
}

}  // namespace rtype::network

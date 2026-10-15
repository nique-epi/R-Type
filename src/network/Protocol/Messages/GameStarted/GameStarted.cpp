#include "GameStarted.hpp"
#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

void encodePayload([[maybe_unused]] ByteWriter& writer,
                   [[maybe_unused]] const GameStarted& message) {}

GameStarted decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<GameStarted> type) {
  const ByteReader reader{payload};
  requireFullyRead(reader);
  return GameStarted{};
}

}  // namespace rtype::network

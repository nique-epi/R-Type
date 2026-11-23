#include "GameStarted.hpp"
#include <cstddef>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

void validate(const GameStarted& message) {
  requireValidBiome(message.biome);
  requireScrollValue(SCROLL_POSITION_FIELD, message.scrollPosition);
  requireFiniteValue(BIOME_SINCE_FIELD, message.biomeSince);
}

}  // namespace

void encodePayload(ByteWriter& writer, const GameStarted& message) {
  validate(message);
  writer.writeString(message.biome);
  writer.writeFloat(message.scrollPosition);
  writer.writeFloat(message.biomeSince);
}

GameStarted decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<GameStarted> type) {
  ByteReader reader{payload};
  std::string biome = reader.readString();
  const float scrollPosition = reader.readFloat();
  const float biomeSince = reader.readFloat();
  requireFullyRead(reader);
  GameStarted message{.biome = std::move(biome),
                      .scrollPosition = scrollPosition,
                      .biomeSince = biomeSince};
  validate(message);
  return message;
}

}  // namespace rtype::network

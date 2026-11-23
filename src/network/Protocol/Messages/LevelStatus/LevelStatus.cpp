#include "LevelStatus.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

void validate(const LevelStatus& message) {
  requireValidBiome(message.biome);
  requireScrollValue(SCROLL_POSITION_FIELD, message.scrollPosition);
  requireScrollValue(SCROLL_SPEED_FIELD, message.scrollSpeed);
  requireFiniteValue(BIOME_SINCE_FIELD, message.biomeSince);
  if (message.bandTop >= message.bandBottom) {
    throw InvalidFieldValueException(BAND_FIELD, message.bandTop);
  }
}

}  // namespace

void encodePayload(ByteWriter& writer, const LevelStatus& message) {
  validate(message);
  writer.writeString(message.biome);
  writer.writeFloat(message.scrollPosition);
  writer.writeFloat(message.scrollSpeed);
  writer.writeFloat(message.biomeSince);
  writer.writeUint16(message.transitionLeft);
  writer.writeUint16(message.bandTop);
  writer.writeUint16(message.bandBottom);
  writer.writeUint8(message.continuesLeft);
}

LevelStatus decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<LevelStatus> type) {
  ByteReader reader{payload};
  std::string biome = reader.readString();
  const float scrollPosition = reader.readFloat();
  const float scrollSpeed = reader.readFloat();
  const float biomeSince = reader.readFloat();
  const std::uint16_t transitionLeft = reader.readUint16();
  const std::uint16_t bandTop = reader.readUint16();
  const std::uint16_t bandBottom = reader.readUint16();
  const std::uint8_t continuesLeft = reader.readUint8();
  requireFullyRead(reader);
  LevelStatus message{.biome = std::move(biome),
                      .scrollPosition = scrollPosition,
                      .scrollSpeed = scrollSpeed,
                      .biomeSince = biomeSince,
                      .transitionLeft = transitionLeft,
                      .bandTop = bandTop,
                      .bandBottom = bandBottom,
                      .continuesLeft = continuesLeft};
  validate(message);
  return message;
}

}  // namespace rtype::network

#include "LevelStatus.hpp"
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include "BiomeName.hpp"
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::network {

namespace {

/** @brief Refuses @p value, the field @p field of a scroll, when it is not a
 * finite number from 0; the exception holds its bits. */
void requireScrollValue(std::string_view field, float value) {
  if (!std::isfinite(value) || value < 0.0F) {
    throw InvalidFieldValueException(field,
                                     std::bit_cast<std::uint32_t>(value));
  }
}

void validate(const LevelStatus& message) {
  if (!message.biome.empty() && !identifiers::isBiomeName(message.biome)) {
    throw InvalidFieldValueException(BIOME_FIELD, message.biome.size());
  }
  requireScrollValue(SCROLL_POSITION_FIELD, message.scrollPosition);
  requireScrollValue(SCROLL_SPEED_FIELD, message.scrollSpeed);
}

}  // namespace

void encodePayload(ByteWriter& writer, const LevelStatus& message) {
  validate(message);
  writer.writeString(message.biome);
  writer.writeFloat(message.scrollPosition);
  writer.writeFloat(message.scrollSpeed);
}

LevelStatus decodePayload(
    std::span<const std::byte> payload,
    [[maybe_unused]] std::type_identity<LevelStatus> type) {
  ByteReader reader{payload};
  std::string biome = reader.readString();
  const float scrollPosition = reader.readFloat();
  const float scrollSpeed = reader.readFloat();
  requireFullyRead(reader);
  LevelStatus message{.biome = std::move(biome),
                      .scrollPosition = scrollPosition,
                      .scrollSpeed = scrollSpeed};
  validate(message);
  return message;
}

}  // namespace rtype::network

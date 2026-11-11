#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief A body plays once the effect its sheet names `effect`, in
 * `[appearance.effects]`. Guaranteed: sent again until acknowledged, for
 * GUARANTEED_MESSAGE_LIFETIME at most. A receiver that knows no such body, or
 * no such effect in the sheet of its type, plays nothing.
 */
struct EffectPlayed {
  static constexpr MessageType TYPE = MessageType::EffectPlayed;

  std::uint32_t entityId{};
  std::string effect;

  bool operator==(const EffectPlayed&) const = default;
};

/**
 * @brief Writes the entity id, then the name of the effect as a short string.
 * @throws InvalidFieldValueException when the entity id is NO_ENTITY_ID, and
 * InvalidContentNameException when the effect is no name of the content
 * (identifiers::isContentName()); nothing is written then.
 */
void encodePayload(ByteWriter& writer, const EffectPlayed& message);

/**
 * @brief Reads an EffectPlayed from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException when the entity id is NO_ENTITY_ID.
 * @throws InvalidContentNameException when the effect is no name of the
 * content.
 * @throws TrailingBytesException when bytes follow the name of the effect.
 */
[[nodiscard]] EffectPlayed decodePayload(std::span<const std::byte> payload,
                                         std::type_identity<EffectPlayed> type);

}  // namespace rtype::network

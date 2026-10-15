#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief Why an entity disappears: Killed shows an explosion, Removed shows
 * nothing. The values are the ones of the protocol page.
 */
enum class DestructionReason : std::uint8_t {
  Killed = 1,
  Removed = 2,
};

/**
 * @brief An entity disappears. Sent three times, 50 ms apart; a copy naming
 * an entity the client no longer has changes nothing.
 */
struct EntityDestroyed {
  static constexpr MessageType TYPE = MessageType::EntityDestroyed;

  std::uint32_t entityId;
  DestructionReason reason;

  bool operator==(const EntityDestroyed&) const = default;
};

/**
 * @brief Writes the entity id, then the reason.
 * @throws InvalidFieldValueException when the entity id is NO_ENTITY_ID or the
 * reason is not one of DestructionReason; nothing is written then.
 */
void encodePayload(ByteWriter& writer, const EntityDestroyed& message);

/**
 * @brief Reads an EntityDestroyed from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException when the entity id is NO_ENTITY_ID or the
 * reason is not one of DestructionReason.
 * @throws TrailingBytesException when bytes follow the reason.
 */
[[nodiscard]] EntityDestroyed decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<EntityDestroyed> type);

}  // namespace rtype::network

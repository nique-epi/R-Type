#pragma once

#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"
#include "ShipLook.hpp"

namespace rtype::network {

/**
 * @brief The items the ship of the player wears, as the player chose them in
 * the hangar. The client sends it when the look changes in the waiting room,
 * and again until a PlayerLooks shows it: an absolute value, never a change.
 */
struct ShipLookChoice {
  static constexpr MessageType TYPE = MessageType::ShipLookChoice;

  identifiers::ShipLook look;

  bool operator==(const ShipLookChoice&) const = default;
};

/** @brief Writes the look as head, eyes, face, one byte each.
 * @throws InvalidFieldValueException when a slot holds a number it may not. */
void encodePayload(ByteWriter& writer, const ShipLookChoice& message);

/**
 * @brief Reads a ShipLookChoice from the whole of @p payload.
 * @throws BufferUnderflowException when it holds fewer than three bytes.
 * @throws InvalidFieldValueException naming head, eyes or face when a byte is
 * not a number its slot may hold.
 * @throws TrailingBytesException when bytes follow them.
 */
[[nodiscard]] ShipLookChoice decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<ShipLookChoice> type);

}  // namespace rtype::network

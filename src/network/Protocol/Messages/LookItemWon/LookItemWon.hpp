#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The item of the locker the player picked up in the game. The server
 * sends it, guaranteed, to that player alone.
 */
struct LookItemWon {
  static constexpr MessageType TYPE = MessageType::LookItemWon;

  /** @brief The item's number: 1 to SHIP_LOOK_ITEM_COUNT. */
  std::uint8_t item{};

  bool operator==(const LookItemWon&) const = default;
};

/**
 * @brief Writes the item number as a u8.
 * @throws InvalidFieldValueException when it names no item of the catalog.
 */
void encodePayload(ByteWriter& writer, const LookItemWon& message);

/**
 * @brief Reads a LookItemWon from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is empty.
 * @throws InvalidFieldValueException when the number names no item.
 * @throws TrailingBytesException when bytes follow it.
 */
[[nodiscard]] LookItemWon decodePayload(std::span<const std::byte> payload,
                                        std::type_identity<LookItemWon> type);

}  // namespace rtype::network

#pragma once

#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief Whether the player is ready to start. The client sends it when the
 * player changes it, and again while LobbyState shows another value, so a
 * lost one is corrected within 500 ms.
 */
struct ReadyState {
  static constexpr MessageType TYPE = MessageType::ReadyState;

  bool ready;

  bool operator==(const ReadyState&) const = default;
};

/**
 * @brief Writes ready as a bool of the protocol.
 */
void encodePayload(ByteWriter& writer, const ReadyState& message);

/**
 * @brief Reads a ReadyState from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is empty.
 * @throws InvalidBoolException when the byte is neither 0 nor 1.
 * @throws TrailingBytesException when bytes follow it.
 */
[[nodiscard]] ReadyState decodePayload(std::span<const std::byte> payload,
                                       std::type_identity<ReadyState> type);

}  // namespace rtype::network

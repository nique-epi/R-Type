#pragma once

#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief Sent after one second without sending anything else, so the other
 * side knows its peer is still there. It has no payload.
 */
struct Heartbeat {
  static constexpr MessageType TYPE = MessageType::Heartbeat;

  bool operator==(const Heartbeat&) const = default;
};

/**
 * @brief Writes nothing: the message has no payload.
 */
void encodePayload(ByteWriter& writer, const Heartbeat& message);

/**
 * @brief Reads a Heartbeat from @p payload, which must be empty.
 * @throws TrailingBytesException when @p payload is not empty.
 */
[[nodiscard]] Heartbeat decodePayload(std::span<const std::byte> payload,
                                      std::type_identity<Heartbeat> type);

}  // namespace rtype::network

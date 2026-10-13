#pragma once

#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief One side leaves: the client when the player closes it, the server when
 * it shuts down. Sent three times, 50 ms apart, since nothing repeats it. It
 * has no payload.
 */
struct Disconnection {
  static constexpr MessageType TYPE = MessageType::Disconnection;

  bool operator==(const Disconnection&) const = default;
};

/**
 * @brief Writes nothing: the message has no payload.
 */
void encodePayload(ByteWriter& writer, const Disconnection& message);

/**
 * @brief Reads a Disconnection from @p payload, which must be empty.
 * @throws TrailingBytesException when @p payload is not empty.
 */
[[nodiscard]] Disconnection decodePayload(
    std::span<const std::byte> payload, std::type_identity<Disconnection> type);

}  // namespace rtype::network

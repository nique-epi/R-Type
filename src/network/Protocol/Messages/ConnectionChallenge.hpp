#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The server's answer to a request without a valid cookie: the cookie
 * the client sends back to prove it owns its address. The server keeps
 * nothing when it sends it.
 */
struct ConnectionChallenge {
  static constexpr MessageType TYPE = MessageType::ConnectionChallenge;

  std::uint64_t cookie;

  bool operator==(const ConnectionChallenge&) const = default;
};

/**
 * @brief Writes the cookie.
 */
void encodePayload(ByteWriter& writer, const ConnectionChallenge& message);

/**
 * @brief Reads a ConnectionChallenge from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws TrailingBytesException when bytes follow the cookie.
 */
[[nodiscard]] ConnectionChallenge decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<ConnectionChallenge> type);

}  // namespace rtype::network

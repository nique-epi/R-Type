#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The master asks a game server to show that its UDP port can be
 * reached, outside any session: it sends the secret the server registered
 * with and a token, and the server sends the token back in its next heartbeat
 * over HTTP, never over UDP.
 *
 * The server keeps the token only when the secret is its own, so a forged
 * probe never replaces the token of the master. It answers nothing: a probe
 * makes it send no byte.
 */
struct MasterProbe {
  static constexpr MessageType TYPE = MessageType::MasterProbe;

  /** @brief The probeSecret of the registration of the server. */
  std::uint64_t secret{};
  /** @brief What the server sends back as the probeToken of its next
   * heartbeat. */
  std::uint64_t token{};

  bool operator==(const MasterProbe&) const = default;
};

/** @brief Writes secret, then token. */
void encodePayload(ByteWriter& writer, const MasterProbe& message);

/**
 * @brief Reads a MasterProbe from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws TrailingBytesException when bytes follow the token.
 */
[[nodiscard]] MasterProbe decodePayload(std::span<const std::byte> payload,
                                        std::type_identity<MasterProbe> type);

}  // namespace rtype::network

#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The supervisor tells a player whose connection request carried a valid
 * cookie to ask again at another port of the same address, where the game that
 * has a place for it runs. It reserves nothing, so it may be sent at once.
 */
struct ConnectionRedirect {
  static constexpr MessageType TYPE = MessageType::ConnectionRedirect;

  std::uint16_t port{};

  bool operator==(const ConnectionRedirect&) const = default;
};

/**
 * @brief Writes the port.
 * @throws InvalidFieldValueException when the port is below LOWEST_PORT;
 * nothing is written then.
 */
void encodePayload(ByteWriter& writer, const ConnectionRedirect& message);

/**
 * @brief Reads a ConnectionRedirect from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload holds fewer than two bytes.
 * @throws TrailingBytesException when bytes follow the port.
 * @throws InvalidFieldValueException when the port is below LOWEST_PORT.
 */
[[nodiscard]] ConnectionRedirect decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<ConnectionRedirect> type);

}  // namespace rtype::network

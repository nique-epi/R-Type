#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief Why the server refuses a connection. The values are the ones of the
 * protocol page; any other byte is invalid.
 */
enum class RefusalReason : std::uint8_t {
  GameFull = 1,
  GameInProgress = 2,
  IncompatibleVersion = 3,
};

/**
 * @brief The server refuses a connection, and says why. It reserves nothing,
 * so it may be sent at once.
 */
struct ConnectionRefused {
  static constexpr MessageType TYPE = MessageType::ConnectionRefused;

  RefusalReason reason;

  bool operator==(const ConnectionRefused&) const = default;
};

/**
 * @brief Writes the reason.
 * @throws InvalidFieldValueException when the reason is not one of
 * RefusalReason; nothing is written then.
 */
void encodePayload(ByteWriter& writer, const ConnectionRefused& message);

/**
 * @brief Reads a ConnectionRefused from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is empty.
 * @throws InvalidFieldValueException when the reason is not one of
 * RefusalReason.
 * @throws TrailingBytesException when bytes follow the reason.
 */
[[nodiscard]] ConnectionRefused decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<ConnectionRefused> type);

}  // namespace rtype::network

#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief What a client sends to join a game, every 500 ms until it gets an
 * answer: the cookie of the server's challenge, 0 until it has one, and the
 * nickname of the player.
 */
struct ConnectionRequest {
  static constexpr MessageType TYPE = MessageType::ConnectionRequest;

  std::uint64_t cookie;
  std::string nickname;

  bool operator==(const ConnectionRequest&) const = default;
};

/**
 * @brief Writes the cookie, then the nickname with its length.
 * @throws InvalidNicknameException when the nickname is empty, longer than
 * MAXIMUM_NICKNAME_SIZE or not UTF-8; nothing is written then.
 */
void encodePayload(ByteWriter& writer, const ConnectionRequest& message);

/**
 * @brief Reads a ConnectionRequest from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidNicknameException when the nickname is empty, longer than
 * MAXIMUM_NICKNAME_SIZE or not UTF-8.
 * @throws TrailingBytesException when bytes follow the nickname.
 */
[[nodiscard]] ConnectionRequest decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<ConnectionRequest> type);

}  // namespace rtype::network

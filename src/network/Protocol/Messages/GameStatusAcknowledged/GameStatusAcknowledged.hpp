#pragma once

#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief A supervisor tells a game process it received its latest GameStatus,
 * so the game knows its supervisor is still there. It has no payload: a
 * datagram that holds only it is too small for the compression of the
 * datagrams of a session to shorten it, and a game process drops a compressed
 * datagram, which a supervisor's Heartbeat, with its fields, would have been.
 */
struct GameStatusAcknowledged {
  static constexpr MessageType TYPE = MessageType::GameStatusAcknowledged;

  bool operator==(const GameStatusAcknowledged&) const = default;
};

/**
 * @brief Writes nothing: the message has no payload.
 */
void encodePayload(ByteWriter& writer, const GameStatusAcknowledged& message);

/**
 * @brief Reads a GameStatusAcknowledged from @p payload, which must be empty.
 * @throws TrailingBytesException when @p payload is not empty.
 */
[[nodiscard]] GameStatusAcknowledged decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<GameStatusAcknowledged> type);

}  // namespace rtype::network

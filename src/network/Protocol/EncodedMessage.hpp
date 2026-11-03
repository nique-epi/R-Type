#pragma once

#include <cstddef>
#include <vector>
#include "MessageType.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief A message ready to send: its type and the bytes of its payload, at
 * most MESSAGE_PAYLOAD_LIMIT of them.
 */
struct EncodedMessage {
  MessageType type{};
  std::vector<std::byte> payload;
};

/**
 * @returns The bytes @p message takes in a datagram: its type and the size of
 * its payload (MESSAGE_PREFIX_SIZE), then its payload.
 */
[[nodiscard]] constexpr std::size_t encodedSize(const EncodedMessage& message) {
  return MESSAGE_PREFIX_SIZE + message.payload.size();
}

}  // namespace rtype::network

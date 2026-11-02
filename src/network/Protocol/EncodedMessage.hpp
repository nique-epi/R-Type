#pragma once

#include <cstddef>
#include <vector>
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief A message ready to send: its type and the bytes of its payload, at
 * most MESSAGE_PAYLOAD_LIMIT of them.
 */
struct EncodedMessage {
  MessageType type{};
  std::vector<std::byte> payload;
};

}  // namespace rtype::network

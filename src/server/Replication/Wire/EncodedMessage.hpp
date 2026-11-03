#pragma once

#include <cstddef>
#include <vector>
#include "MessageType.hpp"

namespace rtype::server {

/**
 * @brief A message ready to send: its type, the bytes of its payload, and how
 * many times it is sent.
 */
struct EncodedMessage {
  network::MessageType type;
  std::vector<std::byte> payload;
  std::size_t copies;
};

}  // namespace rtype::server

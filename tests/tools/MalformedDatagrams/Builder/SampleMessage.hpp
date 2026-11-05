#pragma once

#include <cstddef>
#include <vector>
#include "MessageType.hpp"

namespace rtype::tools {

/**
 * @brief A valid message of the protocol as a datagram carries it: its type
 * and its payload. Every malformed message starts from one.
 */
struct SampleMessage {
  network::MessageType type{};
  std::vector<std::byte> payload;
};

}  // namespace rtype::tools

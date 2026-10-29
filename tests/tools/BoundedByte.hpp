#pragma once

#include <cstddef>
#include "MessageType.hpp"

namespace rtype::tools {

/**
 * @brief Where a message holds a one-byte field the protocol bounds below 255,
 * such as a bool, a player number or a reason: the type of the message, and
 * the offset of the field in its payload.
 */
struct BoundedByte {
  network::MessageType type{};
  std::size_t offset{};
};

}  // namespace rtype::tools

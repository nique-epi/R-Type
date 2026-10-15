#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace rtype::network {

/**
 * @brief One message of a received datagram: its type byte as it arrived, so
 * an unknown type is kept, and a view of its payload.
 *
 * The payload points into the datagram it was split from, and is valid as long
 * as that datagram is.
 */
struct MessageFrame {
  std::uint8_t type;
  std::span<const std::byte> payload;
};

}  // namespace rtype::network

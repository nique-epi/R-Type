#pragma once

#include <cstdint>
#include <string>

namespace rtype::network {

/**
 * @brief Where a datagram comes from or goes to: an IP address and a UDP port.
 *
 * The address is text, the way Asio prints it ("127.0.0.1", "::1"), so that
 * the code outside the network module never names an Asio type. A default
 * Endpoint, such as an empty slot of a queue holds, has no address and port 0.
 */
struct Endpoint {
  std::string address;
  std::uint16_t port{};

  bool operator==(const Endpoint&) const = default;
};

}  // namespace rtype::network

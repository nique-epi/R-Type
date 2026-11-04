#pragma once

#include <cstdint>
#include <string>

namespace rtype::master {

/** @brief Where the HTTP server of the master listens. */
struct HttpEndpoint {
  /** @brief An IPv4 address of the machine, or ALL_INTERFACES. */
  std::string address;
  /** @brief The TCP port; network::ANY_PORT lets the system choose one. */
  std::uint16_t port{};
};

}  // namespace rtype::master

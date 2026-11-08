#pragma once

#include <string>

namespace rtype::network {

/** @brief The addresses a request names its client by. */
struct PeerAddresses {
  /** @brief The IPv4 address of the TCP peer. */
  std::string peer;
  /** @brief The value of the `X-Forwarded-For` header; empty without one. */
  std::string forwardedFor;
};

}  // namespace rtype::network

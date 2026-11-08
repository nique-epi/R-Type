#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief Whether an HttpServer reads the client of a request in the
 * `X-Forwarded-For` header that a reverse proxy on the same machine writes.
 */
enum class ForwardedAddress : std::uint8_t {
  /** @brief The client is always the TCP peer. */
  Ignored,
  /**
   * @brief A request whose TCP peer is LOOPBACK_IPV4_ADDRESS comes from the
   * proxy, and its client is the last address of its `X-Forwarded-For`.
   */
  TrustedFromLoopback
};

}  // namespace rtype::network

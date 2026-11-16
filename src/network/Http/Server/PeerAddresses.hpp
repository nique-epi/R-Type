#pragma once

#include <string>

namespace rtype::network {

/**
 * @brief What a request says of its client: the addresses it names it by,
 * and the scheme it reached a proxy with.
 */
struct PeerAddresses {
  /** @brief The IPv4 address of the TCP peer. */
  std::string peer;
  /** @brief The value of the `X-Forwarded-For` header; empty without one. */
  std::string forwardedFor;
  /** @brief The value of the `X-Forwarded-Proto` header; empty without one.
   */
  std::string forwardedProto;
};

}  // namespace rtype::network

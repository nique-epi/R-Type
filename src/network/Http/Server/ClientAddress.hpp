#pragma once

#include <string>
#include "ForwardedAddress.hpp"
#include "PeerAddresses.hpp"

namespace rtype::network {

/**
 * @returns The address of the client of a request: the last address of the
 * `X-Forwarded-For` of @p addresses when @p trust is
 * ForwardedAddress::TrustedFromLoopback, the peer is LOOPBACK_IPV4_ADDRESS
 * and the header names one; the TCP peer otherwise. A proxy appends the
 * address it saw last, so a client cannot choose it, and a client that
 * reaches the server without the proxy is never the loopback.
 */
[[nodiscard]] std::string clientAddressOf(const PeerAddresses& addresses,
                                          ForwardedAddress trust);

}  // namespace rtype::network

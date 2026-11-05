#pragma once

#include <cstdint>

namespace rtype::server {

/**
 * @returns A UDP port nothing is bound to at this moment, found by binding a
 * socket to the port 0 and giving it back. Another program may take it before
 * the caller does: the caller must be ready for its own bind to fail.
 * @throws network::SocketOpenException when no socket can be bound.
 */
[[nodiscard]] std::uint16_t freeUdpPort();

}  // namespace rtype::server

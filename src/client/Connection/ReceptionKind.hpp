#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What the session does with a datagram of the server. */
enum class ReceptionKind : std::uint8_t {
  /** @brief Not for the session: not of this protocol, from another session,
   * a copy or an older one, or cut short. */
  Ignored,
  /** @brief The header is of this protocol with another version. */
  AnotherVersion,
  /** @brief The messages it holds are for the handlers. */
  Read,
};

}  // namespace rtype::client

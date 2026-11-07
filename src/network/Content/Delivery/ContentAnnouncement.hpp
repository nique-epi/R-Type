#pragma once

#include <cstdint>
#include "ContentFingerprint.hpp"

namespace rtype::network {

/**
 * @brief What a game tells an admitted player of the content it plays with:
 * the fingerprint and the size of its content pack, and the TCP port its
 * supervisor serves the pack on, at the address the player joined; 0 when
 * the supervisor serves none.
 */
struct ContentAnnouncement {
  ContentFingerprint fingerprint;
  std::uint32_t size{};
  std::uint16_t port{};

  bool operator==(const ContentAnnouncement&) const = default;
};

}  // namespace rtype::network

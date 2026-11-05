#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief What a RateLimit decides for one datagram: Allowed lets it through;
 * OverCap drops it; NewlyBanned drops it and starts a ban, once per ban;
 * Banned drops it during a ban.
 */
enum class AllowanceVerdict : std::uint8_t {
  Allowed,
  OverCap,
  NewlyBanned,
  Banned,
};

}  // namespace rtype::network

#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief Where a device link stands. Pending until a player decides on the
 * web site, Approved or Denied once one did, Expired once it outlived its
 * lifetime undecided or unclaimed; a denial stays Denied after the expiry.
 */
enum class DeviceLinkStatus : std::uint8_t {
  Pending,
  Approved,
  Denied,
  Expired
};

}  // namespace rtype::network

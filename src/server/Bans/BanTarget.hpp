#pragma once

#include <string>
#include "BanKind.hpp"

namespace rtype::server {

/** @brief Who a ban bars: a nickname the protocol accepts, or an IPv4 address
 * as network::ipv4AddressFromText() writes it. */
struct BanTarget {
  BanKind kind{BanKind::Nickname};
  std::string text;

  bool operator==(const BanTarget&) const = default;
};

}  // namespace rtype::server

#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Where the player wants to be: seated in its room, or up from it. A
 * value, so the newest one asked is the one the master ends with. */
struct CasinoSeat {
  std::uint8_t room{};
  bool seated{};

  bool operator==(const CasinoSeat&) const = default;
};

}  // namespace rtype::client

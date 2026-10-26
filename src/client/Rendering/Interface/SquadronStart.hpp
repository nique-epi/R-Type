#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How the squadron of the title screen starts: its box opening and
 * its ships flying in, or already in place, as the intro leaves them. */
enum class SquadronStart : std::uint8_t {
  FlyingIn,
  InPlace,
};

}  // namespace rtype::client

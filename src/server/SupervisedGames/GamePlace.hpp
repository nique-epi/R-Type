#pragma once

#include <cstdint>
#include "PlaceKind.hpp"

namespace rtype::server {

/** @brief Where a new player goes. */
struct GamePlace {
  PlaceKind kind{PlaceKind::Full};
  /** @brief The port of the game, for Redirect and Wait; 0 for Open and
   * Full. */
  std::uint16_t port{};
};

}  // namespace rtype::server

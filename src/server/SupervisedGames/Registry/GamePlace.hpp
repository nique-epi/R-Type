#pragma once

#include <cstdint>
#include "ConnectionRefused.hpp"
#include "PlaceKind.hpp"

namespace rtype::server {

/** @brief Where a new player goes. */
struct GamePlace {
  PlaceKind kind{PlaceKind::Refuse};
  /** @brief The port of the game, for Redirect and Wait; 0 for Open and
   * Refuse. */
  std::uint16_t port{};
  /** @brief Why the player is refused, for Refuse; ServerFull otherwise, and
   * read by no one. */
  network::RefusalReason refusal{network::RefusalReason::ServerFull};
};

}  // namespace rtype::server

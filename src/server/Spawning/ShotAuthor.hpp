#pragma once

#include <cstdint>
#include <optional>
#include "ProtocolConstants.hpp"

namespace rtype::server {

/**
 * @brief Who fired a projectile, as its EntitySpawned names it: for a missile
 * of a player, that player and the inputNumber of the newest input applied to
 * its ship; nobody for any other projectile.
 */
struct ShotAuthor {
  std::optional<std::uint8_t> owner;
  std::uint32_t firingInput{network::NO_FIRING_INPUT};
};

}  // namespace rtype::server

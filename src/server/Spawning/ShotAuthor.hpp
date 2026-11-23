#pragma once

#include <cstdint>
#include <optional>
#include "ProtocolConstants.hpp"

namespace rtype::server {

/**
 * @brief Who fired a projectile, as its EntitySpawned names it: for a missile
 * of a player, that player and the inputNumber of the newest input applied to
 * its ship; nobody for any other projectile. Its rank among the projectiles
 * of its shot, from 0, which its EntitySpawned carries too.
 */
struct ShotAuthor {
  std::optional<std::uint8_t> owner;
  std::uint32_t firingInput{network::NO_FIRING_INPUT};
  std::uint8_t shotRank{0};
};

}  // namespace rtype::server

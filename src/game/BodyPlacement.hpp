#pragma once

#include <cstdint>
#include <optional>
#include "Position.hpp"

namespace rtype::game {

/**
 * @brief Where a body appears, and the point its Pilot steers from: the same
 * point for a body that appears where it starts its path, another one for a
 * shard that flies away from the center of the projectile it came out of.
 * A body whose network id was handed out before it appears names it.
 */
struct BodyPlacement {
  Position position{};
  Position origin{};
  std::optional<std::uint32_t> reservedNetworkId = std::nullopt;
};

}  // namespace rtype::game

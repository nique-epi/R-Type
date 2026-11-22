#pragma once

#include <cstdint>

namespace rtype::server {

class PlayerRoster;

/**
 * @brief Adds @p kopecks to the bounty of every player connected to
 * @p roster: what a boss destroyed or a level cleared pays each pilot there,
 * whoever fired. A bounty stops at its largest value instead of wrapping
 * around.
 */
void payEveryPlayer(PlayerRoster& roster, std::uint32_t kopecks);

}  // namespace rtype::server

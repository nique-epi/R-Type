#pragma once

#include <cstdint>

namespace rtype::server {

class PlayerRoster;

/** @returns @p amount, the largest 32-bit value at most: a score or a
 * bounty after a gain, as saturatedCount(std::uint64_t{count} + gain), so it
 * stops instead of wrapping around. */
[[nodiscard]] std::uint32_t saturatedCount(std::uint64_t amount);

/**
 * @brief Adds @p kopecks to the bounty of every player connected to
 * @p roster: what a boss destroyed or a level cleared pays each pilot there,
 * whoever fired. A bounty stops at its largest value instead of wrapping
 * around.
 */
void payEveryPlayer(PlayerRoster& roster, std::uint32_t kopecks);

}  // namespace rtype::server

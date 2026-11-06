#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::game {

/**
 * @brief Component of a projectile that bursts into shards: once it has lived
 * `ticksLeft` more ticks, it is removed and `count` projectiles of the sheet
 * numbered `shardNumber` fly away from where it was, spread evenly around it
 * from the diagonal up and to the right.
 */
struct ShardBurst {
  std::size_t ticksLeft{0};
  std::uint8_t shardNumber{0};
  std::uint8_t count{0};
};

}  // namespace rtype::game

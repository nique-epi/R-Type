#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief The side of an entity type, as the table of ConnectionAccepted writes
 * it: what an entity of that type is in the match, so a receiver knows who it
 * may hit without reading the gameplay of its sheet. The values are the ones
 * of the protocol page and never change.
 */
enum class EntitySide : std::uint8_t {
  Player = 1,
  Enemy = 2,
  PlayerMissile = 3,
  EnemyMissile = 4,
};

}  // namespace rtype::network

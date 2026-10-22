#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Drawing layers, from the first drawn (behind) to the last drawn (in
 * front).
 *
 * The order keeps what can kill a player in sight: the effects (explosions,
 * impacts, muzzle flashes) cover the enemies only, never the ships of the
 * players nor their missiles, the figures of a gain, or the missiles of the
 * enemies, drawn last of the playfield. Interface must stay the last value:
 * the client sizes its per-layer storage from it.
 */
enum class Layer : std::uint8_t {
  Background,
  Enemies,
  Effects,
  Players,
  Popups,
  EnemyMissiles,
  Interface
};

}  // namespace rtype::game

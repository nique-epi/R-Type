#pragma once

namespace rtype::client {

/**
 * @brief Component of the ship of a pilot: its color, 0 to
 * PLAYER_COLOR_COUNT - 1, the player number less FIRST_PLAYER_NUMBER. It
 * colors the outline of an invulnerable ship, its debris and its shockwave.
 * EntityReplicator sets it from the players of each world state.
 */
struct PilotShip {
  int color{};
};

}  // namespace rtype::client

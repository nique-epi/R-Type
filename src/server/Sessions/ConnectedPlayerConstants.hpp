#pragma once

#include <cstdint>

namespace rtype::server {

/**
 * @brief Lives a player starts each game with, the ship in play included: its
 * ship can be destroyed this many times before the player only watches.
 *
 * Provisional: gameplay will tune it.
 */
constexpr std::uint8_t STARTING_LIVES = 3;

}  // namespace rtype::server

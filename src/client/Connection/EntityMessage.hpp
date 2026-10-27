#pragma once

#include <variant>
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "GameEnded.hpp"
#include "GameEvent.hpp"
#include "WorldState.hpp"

namespace rtype::client {

/**
 * @brief A message of the server about the game in progress, as the game
 * screen applies it: an entity appears, the state of the world at one tick,
 * an entity disappears, something happens to an entity or a player (the first
 * copy of a GameEvent only), or the game is over, with every score (the first
 * copy of GameEnded only).
 */
using EntityMessage = std::variant<network::EntitySpawned, network::WorldState,
                                   network::EntityDestroyed, network::GameEvent,
                                   network::GameEnded>;

}  // namespace rtype::client

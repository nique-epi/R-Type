#pragma once

#include <variant>
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "GameEvent.hpp"
#include "WorldState.hpp"

namespace rtype::client {

/**
 * @brief A message of the server about the entities of the game, as the game
 * screen applies it: an entity appears, the state of the world at one tick,
 * an entity disappears, something happens to an entity or a player (the first
 * copy of a GameEvent only).
 */
using EntityMessage =
    std::variant<network::EntitySpawned, network::WorldState,
                 network::EntityDestroyed, network::GameEvent>;

}  // namespace rtype::client

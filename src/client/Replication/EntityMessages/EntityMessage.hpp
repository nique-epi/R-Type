#pragma once

#include <variant>
#include "BossStatus.hpp"
#include "EnemyAction.hpp"
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "GameEnded.hpp"
#include "GameEvent.hpp"
#include "LevelStatus.hpp"
#include "PlayerLeft.hpp"
#include "WorldState.hpp"

namespace rtype::client {

/**
 * @brief A message of the server about the game in progress, as the game
 * screen applies it: an entity appears, the state of the world at one tick,
 * an entity disappears, something happens to an entity or a player (the first
 * copy of a GameEvent only), a player left the game (every copy of
 * PlayerLeft that arrives), an enemy enters a step of an attack (the first
 * copy of each EnemyAction only), the game is over, with every score (the
 * first copy of GameEnded only), the state of the boss (every BossStatus), or
 * the biome and the scroll of the level (every LevelStatus).
 */
using EntityMessage =
    std::variant<network::EntitySpawned, network::WorldState,
                 network::EntityDestroyed, network::GameEvent,
                 network::PlayerLeft, network::GameEnded, network::EnemyAction,
                 network::BossStatus, network::LevelStatus>;

}  // namespace rtype::client

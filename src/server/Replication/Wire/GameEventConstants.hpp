#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::server {

/**
 * @brief The number of the first GameEvent of a game, see the Game events
 * section of the protocol page.
 */
constexpr std::uint16_t FIRST_EVENT_NUMBER = 1;

/**
 * @brief The names the broadcaster gives the events in the warning it logs
 * when it skips one.
 */
constexpr std::string_view ENTITY_SPAWN_EVENT_NAME = "EntitySpawn";
constexpr std::string_view ENTITY_DESTRUCTION_EVENT_NAME = "EntityDestruction";
constexpr std::string_view ENTITY_DAMAGE_EVENT_NAME = "EntityDamage";
constexpr std::string_view ENEMY_ACTION_STARTED_EVENT_NAME =
    "EnemyActionStarted";
constexpr std::string_view EFFECT_STARTED_EVENT_NAME = "EffectStarted";
constexpr std::string_view PLAYER_HIT_EVENT_NAME = "PlayerHit";
constexpr std::string_view PLAYER_ELIMINATION_EVENT_NAME = "PlayerElimination";
constexpr std::string_view PLAYER_DEPARTURE_EVENT_NAME = "PlayerDeparture";
constexpr std::string_view BOSS_ARRIVAL_EVENT_NAME = "BossArrival";
constexpr std::string_view BOSS_HEALTH_CHANGED_EVENT_NAME = "BossHealthChanged";

}  // namespace rtype::server

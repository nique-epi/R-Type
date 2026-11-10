#pragma once

#include <array>
#include <cstddef>
#include <string_view>

namespace rtype::game {

/** @brief The table a script reaches the event bus of the game through, and
 * its functions: `bus.on(name, function)` and `bus.emit(name, fields)`. */
constexpr std::string_view EVENT_BUS_TABLE = "bus";
constexpr std::string_view ON_FUNCTION_NAME = "on";
constexpr std::string_view EMIT_FUNCTION_NAME = "emit";

/** @brief Where `bus.on` and `bus.emit` take their arguments. */
constexpr int EVENT_NAME_ARGUMENT = 1;
constexpr int EVENT_FUNCTION_ARGUMENT = 2;
constexpr int EVENT_FIELDS_ARGUMENT = 2;

/** @brief The events of the game a script may subscribe to, the only ones it
 * sees; any other name is an event the scripts publish themselves. */
constexpr std::string_view ENTITY_DESTROYED_EVENT_NAME = "entity_destroyed";
constexpr std::string_view ENTITY_DAMAGED_EVENT_NAME = "entity_damaged";
constexpr std::string_view ENEMY_ACTION_STARTED_EVENT_NAME =
    "enemy_action_started";
constexpr std::string_view BOSS_HEALTH_CHANGED_EVENT_NAME =
    "boss_health_changed";
constexpr std::array<std::string_view, 4> GAME_EVENT_NAMES{
    ENTITY_DESTROYED_EVENT_NAME, ENTITY_DAMAGED_EVENT_NAME,
    ENEMY_ACTION_STARTED_EVENT_NAME, BOSS_HEALTH_CHANGED_EVENT_NAME};

/** @brief The keys of the table a function subscribed to an event of the game
 * reads. */
constexpr const char* EVENT_ENTITY_ID_KEY = "id";
constexpr const char* EVENT_KILLED_KEY = "killed";
constexpr const char* EVENT_ATTACK_KEY = "attack";
constexpr const char* EVENT_PHASE_KEY = "phase";
constexpr const char* EVENT_POINTS_KEY = "points";
constexpr const char* EVENT_FULL_POINTS_KEY = "full";
constexpr const char* EVENT_ENRAGED_KEY = "enraged";

/** @brief How many functions one entity may keep subscribed: a function that
 * returns starts again at the next tick, and one that subscribed on each of
 * them would grow without end. */
constexpr std::size_t MAXIMUM_SUBSCRIPTIONS_PER_ENTITY = 16;

/** @brief How many values an event a script publishes may carry. */
constexpr std::size_t MAXIMUM_SCRIPT_EVENT_FIELDS = 16;

/** @brief What `bus.on` and `bus.emit` refuse with, and what stops the script
 * of an entity whose subscribed function waits. */
constexpr const char* SUBSCRIPTION_OUTSIDE_ENTITY_MESSAGE =
    "bus.on only runs in the function of a body, during a game";
constexpr const char* TOO_MANY_SUBSCRIPTIONS_MESSAGE =
    "a body subscribed too many functions: subscribe once, before a loop "
    "that waits";
constexpr const char* EMISSION_OUTSIDE_GAME_MESSAGE =
    "events are only published during a game";
constexpr const char* EMISSION_OF_GAME_EVENT_MESSAGE =
    "only the game publishes its own events";
constexpr const char* TOO_MANY_EVENT_FIELDS_MESSAGE =
    "an event carries too many values";
constexpr const char* WRONG_EVENT_FIELD_MESSAGE =
    "the values of an event are numbers, booleans or strings, under string "
    "keys";
constexpr const char* WAIT_IN_SUBSCRIBED_FUNCTION_MESSAGE =
    "a function subscribed to an event cannot wait";
constexpr const char* SUBSCRIBED_FUNCTION_FAILURE_PREFIX =
    "a function subscribed to an event failed: ";

}  // namespace rtype::game

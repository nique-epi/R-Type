#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::game {

/** @brief The table a script registers its behaviours through, and its
 * function: `behaviours.register(name, settings, steer)`. */
constexpr std::string_view BEHAVIOURS_TABLE = "behaviours";
constexpr std::string_view REGISTER_FUNCTION_NAME = "register";
constexpr std::string_view WAIT_FUNCTION_NAME = "wait";
constexpr std::string_view SPAWN_FUNCTION_NAME = "spawn";
constexpr std::string_view DESTROY_FUNCTION_NAME = "destroy";

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

/** @brief The folder of a content folder that holds its server scripts, and
 * their extension. */
constexpr std::string_view SCRIPT_FOLDER_IN_CONTENT = "server";
constexpr std::string_view SCRIPT_FILE_EXTENSION = ".luau";

/** @brief The keys of the view a script reads. */
constexpr const char* VIEW_ID_KEY = "id";
constexpr const char* VIEW_POSITION_KEY = "position";
constexpr const char* VIEW_ORIGIN_KEY = "origin";
constexpr const char* VIEW_AGE_KEY = "age_ms";
constexpr const char* VIEW_ELAPSED_KEY = "elapsed_ms";
constexpr const char* VIEW_TOP_SPEED_KEY = "top_speed";
constexpr const char* VIEW_SETTINGS_KEY = "settings";
constexpr const char* VIEW_ACTION_KEY = "action";
constexpr const char* VIEW_ACTION_AGE_KEY = "action_age_ms";
constexpr const char* VIEW_NEAREST_SHIP_KEY = "nearest_ship";
constexpr const char* VIEW_WOUNDED_KEY = "wounded";
constexpr const char* VIEW_HEALTH_KEY = "health";
constexpr const char* VIEW_ENRAGED_KEY = "enraged";
constexpr const char* VIEW_ACTIONS_ENDED_KEY = "actions_ended";

/** @brief The keys of a point and of an action, in the view and the intent. */
constexpr const char* POINT_X_KEY = "x";
constexpr const char* POINT_Y_KEY = "y";
constexpr const char* ACTION_ATTACK_KEY = "attack";
constexpr const char* ACTION_PHASE_KEY = "phase";

/** @brief The keys of the intent a script returns. */
constexpr const char* INTENT_DIRECTION_KEY = "direction";
constexpr const char* INTENT_SHOOTING_KEY = "shooting";
constexpr const char* INTENT_STARTS_ACTION_KEY = "starts_action";
constexpr const char* INTENT_ENDS_ACTION_KEY = "ends_action";
constexpr const char* INTENT_JUMPS_TO_KEY = "jumps_to";
constexpr const char* INTENT_COLLISION_BOX_KEY = "collision_box";
constexpr const char* INTENT_TOP_SPEED_KEY = "top_speed";

/** @brief The keys of a collision box in the intent. */
constexpr const char* BOX_WIDTH_KEY = "width";
constexpr const char* BOX_HEIGHT_KEY = "height";

/** @brief The keys of a setting a script declares. */
constexpr const char* SETTING_NAME_KEY = "name";
constexpr const char* SETTING_LOWEST_KEY = "lowest";
constexpr const char* SETTING_HIGHEST_KEY = "highest";

/** @brief The names of the phases of an action, in the order of EnemyPhase. */
constexpr std::array<std::string_view, 4> PHASE_NAMES{"signal", "strike",
                                                      "recover", "enraged"};

/** @brief The highest attack number an action may carry. */
constexpr std::int64_t HIGHEST_ATTACK_NUMBER = 255;

}  // namespace rtype::game

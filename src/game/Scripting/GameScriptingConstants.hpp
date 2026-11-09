#pragma once

#include <array>
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

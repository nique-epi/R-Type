#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <string_view>

namespace rtype::game {

/** @brief The components a script reaches through the handle of its body
 * (`e.position`), the only ones it reaches. */
constexpr std::string_view POSITION_COMPONENT_KEY = "position";
constexpr std::string_view HEALTH_COMPONENT_KEY = "health";
constexpr std::string_view FULL_HEALTH_COMPONENT_KEY = "full_health";
constexpr std::string_view COLLISION_BOX_COMPONENT_KEY = "collision_box";
constexpr std::string_view TOP_SPEED_COMPONENT_KEY = "top_speed";
constexpr std::string_view SCORE_COMPONENT_KEY = "score";
constexpr std::string_view GUN_INTERVAL_COMPONENT_KEY = "gun_interval";
constexpr std::array<std::string_view, 7> COMPONENT_KEYS{
    POSITION_COMPONENT_KEY,    HEALTH_COMPONENT_KEY,
    FULL_HEALTH_COMPONENT_KEY, COLLISION_BOX_COMPONENT_KEY,
    TOP_SPEED_COMPONENT_KEY,   SCORE_COMPONENT_KEY,
    GUN_INTERVAL_COMPONENT_KEY};

/** @brief The table of its own a script keeps in the handle of each body, and
 * the field of the hidden table of a handle that holds it. */
constexpr const char* SCRIPT_DATA_KEY = "data";
/** @brief The field of the hidden table of a handle that holds the view of
 * the last step of its body. */
constexpr const char* HANDLE_VIEW_FIELD = "view";

/** @brief The name of the metatable of the handles in the registry of a
 * machine, and what `getmetatable` answers a script about a handle. */
constexpr const char* ENTITY_HANDLE_METATABLE = "rtype.entity";
constexpr const char* LOCKED_METATABLE_TEXT = "locked";
constexpr const char* INDEX_METAMETHOD = "__index";
constexpr const char* NEW_INDEX_METAMETHOD = "__newindex";
constexpr const char* METATABLE_FIELD = "__metatable";

/** @brief Where the metamethods of a handle take their arguments. */
constexpr int HANDLE_ARGUMENT = 1;
constexpr int FIELD_ARGUMENT = 2;
constexpr int WRITTEN_VALUE_ARGUMENT = 3;

/** @brief The lowest health a script may write; the highest is the full
 * health of the body. */
constexpr double LOWEST_WRITTEN_HEALTH = 1.0;
/** @brief The bounds of the score and of the gun interval a script may
 * write. */
constexpr double LOWEST_WRITTEN_SCORE = 0.0;
constexpr double HIGHEST_WRITTEN_SCORE =
    std::numeric_limits<std::uint32_t>::max();
constexpr double LOWEST_WRITTEN_GUN_INTERVAL = 1.0;
constexpr double HIGHEST_WRITTEN_GUN_INTERVAL =
    std::numeric_limits<std::uint32_t>::max();

/** @brief The errors a handle raises in the script that misuses it. */
constexpr std::string_view GONE_BODY_ERROR = "this body is gone";
constexpr std::string_view OTHER_STEP_ERROR =
    "the components of a body are only reached during its own step";
constexpr std::string_view HANDLE_FIELD_PREFIX = "e.";
constexpr std::string_view ABSENT_COMPONENT_ERROR =
    ": this body has no such component";
constexpr std::string_view READ_ONLY_FIELD_ERROR = " cannot be written";
constexpr std::string_view OUTSIDE_GAME_WRITE_ERROR =
    "the components of a body are only written during a game";
constexpr std::string_view DATA_REPLACED_ERROR =
    "e.data cannot be replaced: write in it";
constexpr std::string_view POSITION_REFUSAL =
    "a position is a table of two finite numbers, x and y";
constexpr std::string_view HEALTH_REFUSAL =
    "health is a whole number from 1 to full_health";
constexpr std::string_view COLLISION_BOX_REFUSAL =
    "a collision box is a table of two positive finite numbers, width and "
    "height";
constexpr std::string_view TOP_SPEED_REFUSAL =
    "a top speed is a positive finite number";
constexpr std::string_view SCORE_REFUSAL =
    "a score is a whole number from 0 to 4294967295";
constexpr std::string_view GUN_INTERVAL_REFUSAL =
    "a gun interval is a whole number of ticks from 1 to 4294967295";

}  // namespace rtype::game

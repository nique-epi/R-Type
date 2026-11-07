#pragma once

#include <string_view>

namespace rtype::server::testing {

/** @brief The key that names the boss of a difficulty, the boss it names
 * (the warlord of BossStageTestConstants.hpp) and a sheet that is no boss. */
constexpr std::string_view BOSS_KEY_IN_TESTS = "boss";
constexpr std::string_view WARLORD_AS_A_VALUE = "\"warlord\"";
constexpr std::string_view DRIFTER_AS_A_BOSS = "\"drifter\"";
constexpr std::string_view UNKNOWN_DRIFTER_BOSS_TEXT =
    "unknown boss \"drifter\"";

}  // namespace rtype::server::testing

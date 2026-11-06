#pragma once

#include <span>
#include <string_view>
#include "HandOverRule.hpp"

namespace rtype::server {

/**
 * @returns The rule the supervisor reads server.cfg and its forced lines with:
 * a line belongs to its games when it sets a setting of GAME_SETTING_NAMES,
 * runs a command of GAME_COMMAND_NAMES, or turns on or off a module that is
 * not one of @p supervisorModuleNames (a module of a game process, or a
 * plugin).
 * Every other line is the supervisor's, and one it does not know is refused
 * there.
 *
 * @param supervisorModuleNames The names of the modules of the supervisor,
 * SUPERVISOR_MODULE_NAMES; the rule keeps a copy.
 */
[[nodiscard]] engine::HandOverRule gameLineRule(
    std::span<const std::string_view> supervisorModuleNames);

}  // namespace rtype::server

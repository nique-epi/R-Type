#pragma once

#include <memory>
#include <span>
#include "HandOverRule.hpp"
#include "IModule.hpp"

namespace rtype::server {

/**
 * @returns The rule the supervisor reads server.cfg and its forced lines with:
 * a line belongs to its games when it sets a setting of GAME_SETTING_NAMES,
 * runs a command of GAME_COMMAND_NAMES, or turns on or off a module that is
 * not one of @p supervisorModules (a module of a game process, or a plugin).
 * Every other line is the supervisor's, and one it does not know is refused
 * there.
 *
 * @param supervisorModules The modules of the supervisor; the rule keeps their
 * names, so the list may go once it returns.
 */
[[nodiscard]] engine::HandOverRule gameLineRule(
    std::span<const std::unique_ptr<engine::IModule>> supervisorModules);

}  // namespace rtype::server

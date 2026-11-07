#pragma once

#include <array>
#include "ModuleConstants.hpp"
#include "PluginConstants.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

/**
 * @brief The settings a game process declares and the supervisor does not:
 * a `set` line on one of them, in server.cfg or forced by a launch option of
 * the supervisor, goes to the games.
 */
constexpr std::array GAME_SETTING_NAMES{
    SEED_SETTING_NAME, STATE_RATE_SETTING_NAME, STATE_ENCODING_SETTING_NAME,
    INPUT_DELAY_SETTING_NAME, GAME_STATISTICS_FILE_SETTING_NAME};

/**
 * @brief The commands only a game process runs: the supervisor loads no
 * plugin, its games do.
 */
constexpr std::array GAME_COMMAND_NAMES{engine::PLUGIN_COMMAND_NAME,
                                        engine::PLUGIN_FOLDER_COMMAND_NAME};

/**
 * @brief The names of the modules supervisorModules() lists, in its order,
 * known before they are built: the rule of the lines of the games needs them
 * to read the configuration the games module is built with.
 */
constexpr std::array SUPERVISOR_MODULE_NAMES{
    LOGGER_MODULE_NAME,           NETWORK_MODULE_NAME,
    CONTENT_MODULE_NAME,          SUPERVISED_GAMES_MODULE_NAME,
    SUPERVISOR_LOBBY_MODULE_NAME, INFORMATION_MODULE_NAME,
    MASTER_MODULE_NAME,           ADMIN_CONSOLE_MODULE_NAME};

}  // namespace rtype::server

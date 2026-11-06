#pragma once

#include "GameLaunch.hpp"
#include "GameLaunchSettings.hpp"
#include "ProcessCommand.hpp"

namespace rtype::server {

/**
 * @returns The command that starts the game @p launch describes:
 * `<program> --mode game -p <port> --supervisor <port> --link-token <token>
 * --database <database> --log-file
 * <folder>/r-type_game_<number>.log --configuration
 * <folder>/r-type_games_<supervisor port>.cfg`, where <database> is the
 * database setting of @p settings, or <folder>/r-type_server_<supervisor
 * port>.db while it is empty.
 */
[[nodiscard]] ProcessCommand gameCommand(const GameLaunchSettings& settings,
                                         const GameLaunch& launch);

}  // namespace rtype::server

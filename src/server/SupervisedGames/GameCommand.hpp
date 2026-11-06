#pragma once

#include "GameLaunch.hpp"
#include "GameLaunchSettings.hpp"
#include "ProcessCommand.hpp"

namespace rtype::server {

/**
 * @returns The command that starts the game @p launch describes:
 * `<program> --mode game -p <port> --supervisor <port> --link-token <token>
 * --database <folder>/r-type_server_<supervisor port>.db --log-file
 * <folder>/r-type_game_<number>.log --configuration
 * <folder>/r-type_games_<supervisor port>.cfg`.
 */
[[nodiscard]] ProcessCommand gameCommand(const GameLaunchSettings& settings,
                                         const GameLaunch& launch);

}  // namespace rtype::server

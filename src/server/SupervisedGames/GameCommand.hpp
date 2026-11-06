#pragma once

#include <string_view>
#include "GameLaunch.hpp"
#include "GameLaunchSettings.hpp"
#include "ProcessCommand.hpp"

namespace rtype::server {

/**
 * @returns The command that starts the game @p launch describes:
 * `<program> --mode game -p <port> --supervisor <port> --link-token <token>
 * --database <database> --log-file
 * <folder>/r-type_game_<number>.log --configuration
 * <folder>/r-type_games_<supervisor port>.cfg`, where <database> is
 * @p databaseSetting, the database setting of the supervisor, or
 * <folder>/r-type_server_<supervisor port>.db while it is empty; then the
 * forwarded arguments of @p settings, so a `--no-log-file` among them wins over
 * the journal of the game; then, when
 * @p settings has a traffic file <folder>/<stem><ext>, `--traffic-csv
 * <folder>/<stem>_game_<number><ext>`.
 */
[[nodiscard]] ProcessCommand gameCommand(const GameLaunchSettings& settings,
                                         const GameLaunch& launch,
                                         std::string_view databaseSetting);

}  // namespace rtype::server

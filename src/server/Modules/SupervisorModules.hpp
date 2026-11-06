#pragma once

#include <memory>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "LaunchOptions.hpp"
#include "StopRequest.hpp"

namespace rtype::server {

/**
 * @brief The modules of the supervisor, in the order an Application installs
 * and starts them (and stops them in reverse): the logger, the network, the
 * games, which run one process each, the lobby that sends the players to
 * them, the information that answers who asks how full the server is, and the
 * master, which keeps the master told of the games. A game process is this same
 * executable, run with `--mode game`.
 *
 * @param launchOptions Give the network conditions and the logging options.
 * @param stopRequest Told by the network module when the supervisor fails; it
 * must outlive the modules.
 * @param gameLines The lines of the configuration that belong to the games,
 * which the configuration step fills before the modules start and the games
 * module writes for every game; they must outlive the modules.
 * @throws engine::UnknownExecutablePathException when the system does not tell
 * where the executable is, so the supervisor cannot start games.
 */
[[nodiscard]] std::vector<std::unique_ptr<engine::IModule>> supervisorModules(
    const LaunchOptions& launchOptions, StopRequest& stopRequest,
    const std::vector<std::string>& gameLines);

}  // namespace rtype::server

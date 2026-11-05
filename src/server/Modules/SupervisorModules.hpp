#pragma once

#include <memory>
#include <vector>
#include "IModule.hpp"
#include "LaunchOptions.hpp"
#include "StopRequest.hpp"

namespace rtype::server {

/**
 * @brief The modules of the supervisor, in the order an Application installs
 * and starts them (and stops them in reverse): the logger, the network, the
 * games, which run one process each, and the lobby that sends the players to
 * them. A game process is this same executable, run as a server.
 *
 * @param launchOptions Give the network conditions and the logging options.
 * @param stopRequest Told by the network module when the supervisor fails; it
 * must outlive the modules.
 * @throws engine::UnknownExecutablePathException when the system does not tell
 * where the executable is, so the supervisor cannot start games.
 */
[[nodiscard]] std::vector<std::unique_ptr<engine::IModule>> supervisorModules(
    const LaunchOptions& launchOptions, StopRequest& stopRequest);

}  // namespace rtype::server

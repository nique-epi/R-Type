#pragma once

#include <memory>
#include <span>
#include <string>
#include <vector>
#include "GameLaunchOptions.hpp"
#include "IModule.hpp"
#include "StopRequest.hpp"

namespace rtype::server {

/**
 * @brief The modules of a game process, in the order an Application installs
 * and starts them (and stops them in reverse): the logger, the network, the
 * storage, the games, which host one game, the lobby, the metrics and the
 * link to the supervisor that started the process. This is the one place that
 * lists them.
 *
 * @param launchOptions Give the logging options, the network conditions, the
 * traffic file of the metrics and the supervisor to report to.
 * @param stopRequest Told by the network module when the game fails, and by
 * the link when the supervisor asks it to stop or is gone; it must outlive the
 * modules.
 * @param resources The resources the game starts, from the `ensure` lines of
 * its configuration (see engine::ensuredResources()): their content is laid
 * over the assets (see engine::contentFoldersOf()).
 * @throws EngineException when the system does not tell where the executable
 * is, no assets folder lies next to it or in the folder above, or a resource
 * cannot be started.
 */
[[nodiscard]] std::vector<std::unique_ptr<engine::IModule>> gameProcessModules(
    const GameLaunchOptions& launchOptions, StopRequest& stopRequest,
    std::span<const std::string> resources = {});

}  // namespace rtype::server

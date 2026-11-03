#pragma once

#include <memory>
#include <vector>
#include "IModule.hpp"
#include "LaunchOptions.hpp"
#include "LogLaunchOptions.hpp"
#include "StopRequest.hpp"

namespace rtype::server {

/**
 * @brief The modules of the server, in the order an Application installs and
 * starts them (and stops them in reverse): the logger, the network, the games,
 * the lobby and the metrics. This is the one place that lists them.
 *
 * @param logOptions Applied by the logger module when it starts.
 * @param launchOptions Give the port the network module listens on and the
 * seed of the first game.
 * @param stopRequest Told by the network module when the server fails; it must
 * outlive the modules.
 * @throws EngineException when the system does not tell where the executable
 * is, or no assets folder lies next to it or in the folder above.
 */
[[nodiscard]] std::vector<std::unique_ptr<engine::IModule>> serverModules(
    const logging::LogLaunchOptions& logOptions,
    const LaunchOptions& launchOptions, StopRequest& stopRequest);

}  // namespace rtype::server

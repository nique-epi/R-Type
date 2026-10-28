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
 * starts them (and stops them in reverse): the journal, the network, the games
 * and the lobby. This is the one place that lists them.
 *
 * @param logOptions Applied by the journal module when it starts.
 * @param launchOptions Give the port the network module listens on and the
 * seed of the first game.
 * @param stopRequest Told by the network module when the server fails; it must
 * outlive the modules.
 */
[[nodiscard]] std::vector<std::unique_ptr<engine::IModule>> serverModules(
    const logging::LogLaunchOptions& logOptions,
    const LaunchOptions& launchOptions, StopRequest& stopRequest);

}  // namespace rtype::server

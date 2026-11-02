#pragma once

#include <memory>
#include <vector>
#include "IModule.hpp"
#include "LogLaunchOptions.hpp"
#include "StopRequest.hpp"

namespace rtype::server {

/**
 * @brief The modules of the server, in the order an Application installs and
 * starts them (and stops them in reverse): the logger, the network, the games
 * and the lobby. This is the one place that lists them.
 *
 * The port, the seed and the state rate are settings the modules declare; the
 * configuration (see serverConfiguration()) gives them their values.
 *
 * @param logOptions Applied by the logger module when it starts.
 * @param stopRequest Told by the network module when the server fails; it must
 * outlive the modules.
 */
[[nodiscard]] std::vector<std::unique_ptr<engine::IModule>> serverModules(
    const logging::LogLaunchOptions& logOptions, StopRequest& stopRequest);

}  // namespace rtype::server

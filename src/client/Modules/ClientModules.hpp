#pragma once

#include <memory>
#include <vector>
#include "FrameLoop.hpp"
#include "IModule.hpp"
#include "LaunchOptions.hpp"
#include "LogLaunchOptions.hpp"

namespace rtype::client {

/**
 * @brief The modules of the client, in the order an Application installs and
 * starts them (and stops them in reverse): the logger, the assets, the
 * settings, the window, the connection, the metrics, the sound, the screens,
 * then the modules that add theirs to its registry: the menus, the lobby and
 * the game.
 * This is the one place that lists them. The sound and the metrics are in
 * the list only when the build compiled them (RTYPE_MODULE_AUDIO,
 * RTYPE_MODULE_METRICS); without the sound, the screens play nothing. Listed,
 * the sound stays before the screens, which look for its service when they
 * install.
 *
 * @param logOptions Applied by the logger module when it starts.
 * @param launchOptions Give the server and the nickname the connection screen
 * starts with, whether the client makes sound, and where its traffic goes.
 * @param loop Run by main(); the window module provides it, so the window and
 * the screens can stop it. It must outlive the modules.
 */
[[nodiscard]] std::vector<std::unique_ptr<engine::IModule>> clientModules(
    const logging::LogLaunchOptions& logOptions,
    const LaunchOptions& launchOptions, FrameLoop& loop);

}  // namespace rtype::client

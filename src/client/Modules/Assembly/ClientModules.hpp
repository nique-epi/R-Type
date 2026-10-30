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
 * settings, the window, the connection, the sound and the screens. This is the
 * one place that lists them.
 *
 * @param logOptions Applied by the logger module when it starts.
 * @param launchOptions Give the server and the nickname the connection screen
 * starts with, and whether the client makes sound.
 * @param loop Run by main(); the window module provides it, so the window and
 * the screens can stop it. It must outlive the modules.
 */
[[nodiscard]] std::vector<std::unique_ptr<engine::IModule>> clientModules(
    const logging::LogLaunchOptions& logOptions,
    const LaunchOptions& launchOptions, FrameLoop& loop);

}  // namespace rtype::client

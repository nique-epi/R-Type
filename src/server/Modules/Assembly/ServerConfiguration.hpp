#pragma once

#include "ConfigurationReport.hpp"
#include "ConfigurationStep.hpp"
#include "LaunchOptions.hpp"

namespace rtype::server {

/**
 * @brief The configuration step of the server: it reads server.cfg, then runs
 * the lines the launch options force, so a launch option wins over the file.
 *
 * @param launchOptions Give the lines to force; only the lines are copied.
 * @param report Filled when the step runs, for the program to log once the
 * journal is open; it must outlive the application.
 */
[[nodiscard]] engine::ConfigurationStep serverConfiguration(
    const LaunchOptions& launchOptions, engine::ConfigurationReport& report);

}  // namespace rtype::server

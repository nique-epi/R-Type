#pragma once

#include <string>
#include <vector>
#include "LaunchOption.hpp"

namespace rtype::server {

/**
 * @returns `-p <port>`, which adds to @p forcedLines the line that forces the
 * port setting to a checked port.
 * @throws network::InvalidPortException from the option when the value is not
 * a port from LOWEST_PORT to HIGHEST_PORT.
 */
[[nodiscard]] engine::LaunchOption portOption(
    std::vector<std::string>& forcedLines);

/**
 * @returns `--database <file>`, which adds to @p forcedLines the line that
 * forces the database setting to that file.
 */
[[nodiscard]] engine::LaunchOption databaseOption(
    std::vector<std::string>& forcedLines);

/** @returns The line of the usage that describes `-p`, with its default. */
[[nodiscard]] std::string portOptionEntry();

/** @returns The line of the usage that describes `--database`, with its
 * default. */
[[nodiscard]] std::string databaseOptionEntry();

/** @returns The line of the usage that describes `--help`. */
[[nodiscard]] std::string helpOptionEntry();

}  // namespace rtype::server

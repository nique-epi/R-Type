#pragma once

#include <span>
#include <string>
#include <string_view>
#include "FloodLaunch.hpp"

namespace rtype::tools {

/**
 * @returns What @p arguments ask: SERVER_OPTION <port> floods the server
 * listening on that port of the loopback address, CLIENT_OPTION <port> waits
 * on that port for a client, HELP_OPTION asks for the usage. When both modes
 * are given, the last one wins.
 * @throws UnknownLaunchArgumentException or
 * MissingLaunchOptionValueException, as readLaunchArguments() does.
 * @throws InvalidPortException when a port is not a whole number from
 * LOWEST_PORT to HIGHEST_PORT.
 */
[[nodiscard]] FloodLaunch readFloodLaunch(
    std::span<const std::string_view> arguments);

/**
 * @returns The usage of the tool.
 */
[[nodiscard]] std::string floodUsage();

}  // namespace rtype::tools

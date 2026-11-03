#pragma once

#include <span>
#include <string>
#include <string_view>
#include "BotLaunch.hpp"

namespace rtype::tools {

/**
 * @returns What @p arguments ask, read in one pass from the first to the last:
 * `-h <address>` and `-p <port>` of the server to join, read as the client
 * reads them, BOTS_OPTION <count> from 1 to MAXIMUM_BOT_COUNT, SECONDS_OPTION
 * <seconds> from 1, SEED_OPTION <seed> from 0 to 4294967295, and HELP_OPTION.
 * The last of each option wins; one not given keeps the default of BotLaunch.
 * @throws engine::UnknownLaunchArgumentException or
 * engine::MissingLaunchOptionValueException, as engine::readLaunchArguments()
 * does.
 * @throws network::InvalidIpv4AddressException when the value of `-h` is not
 * an IPv4 address, network::InvalidPortException when the value of `-p` is not
 * a port.
 * @throws engine::InvalidLaunchOptionValueException when a count, a duration
 * or a seed is not a whole number of its range.
 */
[[nodiscard]] BotLaunch readBotLaunch(
    std::span<const std::string_view> arguments);

/** @returns The usage of the tool. */
[[nodiscard]] std::string botUsage();

}  // namespace rtype::tools

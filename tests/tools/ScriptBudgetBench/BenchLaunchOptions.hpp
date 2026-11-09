#pragma once

#include <span>
#include <string>
#include <string_view>
#include "BenchLaunch.hpp"

namespace rtype::tools {

/**
 * @returns What @p arguments ask: SCRIPTED_BODIES_OPTION <count> and
 * BULLETS_OPTION <count>, each from 0 to 4294967295, TICKS_OPTION <count> from
 * 1 to 4294967295, and HELP_OPTION. An option not given keeps the default of
 * BenchLaunch.
 * @throws engine::UnknownLaunchArgumentException or
 * engine::MissingLaunchOptionValueException, as engine::readLaunchArguments()
 * does.
 * @throws engine::InvalidLaunchOptionValueException when a count is not a
 * whole number of its range.
 */
[[nodiscard]] BenchLaunch readBenchLaunch(
    std::span<const std::string_view> arguments);

/** @returns The usage of the tool. */
[[nodiscard]] std::string benchUsage();

}  // namespace rtype::tools

#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ForcedSetting.hpp"
#include "LaunchOption.hpp"

namespace rtype::engine {

/**
 * @returns `--set <name>=<value>`, which appends to @p forcedLines the line of
 * the command `set` that gives the setting its value. @p forcedLines must
 * outlive the option.
 * @throws InvalidLaunchOptionValueException, from its record, when the value
 * is not `name=value`.
 */
[[nodiscard]] LaunchOption setOption(std::vector<std::string>& forcedLines);

/**
 * @returns The option of @p forced, which appends to @p forcedLines the line of
 * the command `set` that gives its setting the value typed after it, as typed.
 * @p forcedLines must outlive the option.
 */
[[nodiscard]] LaunchOption settingOption(ForcedSetting forced,
                                         std::vector<std::string>& forcedLines);

/**
 * @returns The lines of a usage that describe `--set`, for a program that
 * reads @p configurationFile and declares @p settings.
 */
[[nodiscard]] std::string setOptionEntry(
    std::string_view configurationFile,
    std::span<const std::string_view> settings);

}  // namespace rtype::engine

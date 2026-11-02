#pragma once

#include <filesystem>
#include <span>
#include <string>
#include "CommandInterpreter.hpp"
#include "ConfigurationReport.hpp"

namespace rtype::engine {

/**
 * @brief Runs the lines of a configuration file, then the lines forced at
 * launch, as commands of @p interpreter.
 *
 * A missing file is not an error: nothing runs from it and the report says it
 * was not found, so the defaults apply. A refused line is put in the report and
 * the reading goes on with the next one. The forced lines run after the file,
 * so the last word on a setting is the launch option's.
 *
 * @param file The file of commands, one per line.
 * @param forcedLines Lines run after the file, built from the launch options.
 * @param interpreter Runs each line; it must hold the commands the lines use.
 */
[[nodiscard]] ConfigurationReport readConfiguration(
    const std::filesystem::path& file, std::span<const std::string> forcedLines,
    const CommandInterpreter& interpreter);

}  // namespace rtype::engine

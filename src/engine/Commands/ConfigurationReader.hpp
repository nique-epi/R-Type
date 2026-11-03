#pragma once

#include <filesystem>
#include <span>
#include <string>
#include <vector>
#include "CommandInterpreter.hpp"
#include "ConfigurationReport.hpp"
#include "ConfigurationStep.hpp"

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

/**
 * @brief The configuration step of a program: it reads @p file, then runs
 * @p forcedLines, as readConfiguration() does, and keeps what happened in
 * @p report.
 *
 * @param file The file of commands, one per line.
 * @param forcedLines Lines run after the file, built from the launch options.
 * @param report Filled when the step runs, for the program to log once the
 * journal is open; it must outlive the application.
 */
[[nodiscard]] ConfigurationStep configurationStep(
    std::filesystem::path file, std::vector<std::string> forcedLines,
    ConfigurationReport& report);

}  // namespace rtype::engine

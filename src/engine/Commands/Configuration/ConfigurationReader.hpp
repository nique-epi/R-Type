#pragma once

#include <filesystem>
#include <span>
#include <string>
#include <vector>
#include "CommandInterpreter.hpp"
#include "ConfigurationReport.hpp"
#include "ConfigurationSource.hpp"
#include "ConfigurationStep.hpp"

namespace rtype::engine {

/**
 * @brief Runs the lines of the configuration file of @p source, then the lines
 * forced at launch, as commands of @p interpreter, except the lines the rule of
 * @p source hands over.
 *
 * A missing file is not an error: nothing runs from it and the report says it
 * was not found, so the defaults apply. An empty path reads no file. A refused
 * line is put in the report and the reading goes on with the next one. The
 * forced lines run after the file, so the last word on a setting is the launch
 * option's. A line the rule hands over runs nothing, is neither counted nor
 * refused, and is kept in the report as it was written; a blank line or a
 * comment is never handed over.
 *
 * @param source The file, the forced lines and the rule.
 * @param interpreter Runs each line; it must hold the commands the lines use.
 */
[[nodiscard]] ConfigurationReport readConfiguration(
    const ConfigurationSource& source, const CommandInterpreter& interpreter);

/**
 * @brief Reads @p file then @p forcedLines as readConfiguration() does, with
 * no line handed over.
 *
 * @param file The file of commands, one per line.
 * @param forcedLines Lines run after the file, built from the launch options.
 * @param interpreter Runs each line; it must hold the commands the lines use.
 */
[[nodiscard]] ConfigurationReport readConfiguration(
    const std::filesystem::path& file, std::span<const std::string> forcedLines,
    const CommandInterpreter& interpreter);

/**
 * @brief The configuration step of a program: it reads @p source, as
 * readConfiguration() does, and keeps what happened in @p report.
 *
 * An Application runs its step three times, so the rule of @p source applies
 * in each pass: a handed-over `plugin` or `module` line runs in none of them.
 * Each run replaces the report, so it holds the last run once start() returns.
 *
 * @param source The file, the forced lines and the rule.
 * @param report Filled when the step runs, for the program to log once the
 * journal is open; it must outlive the application.
 */
[[nodiscard]] ConfigurationStep configurationStep(ConfigurationSource source,
                                                  ConfigurationReport& report);

/**
 * @brief The configuration step of a program that hands no line over: it reads
 * @p file, then runs @p forcedLines, and keeps what happened in @p report.
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

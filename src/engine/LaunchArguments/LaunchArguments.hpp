#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "LaunchOption.hpp"
#include "LaunchOptionReader.hpp"

namespace rtype::engine {

/**
 * @brief Reads @p arguments once, from the first to the last: an argument
 * that names one of @p options calls its record, with the argument after it
 * when the option takes a value; any other argument is handed to @p readers,
 * in their order, until one of them takes it.
 *
 * The argument after an option that takes a value, one of @p options or one
 * a reader takes, is always its value, even when it looks like an option of
 * either: no reader is ever handed a value. An option given twice is recorded
 * twice, in order, so the last one wins unless its record decides otherwise.
 *
 * @param arguments the arguments a program was launched with, without its
 *        name (see processArguments()).
 * @param options every option the program reads itself.
 * @param readers the readers of the libraries that read their own options;
 *        none by default.
 * @throws UnknownLaunchArgumentException for an argument that names none of
 *         @p options and that no reader takes, a value given without its
 *         option included.
 * @throws MissingLaunchOptionValueException when an option of @p options
 *         that takes a value is the last argument.
 * @throws whatever a record or a reader throws to refuse an option or a
 *         value.
 */
void readLaunchArguments(std::span<const std::string_view> arguments,
                         std::span<const LaunchOption> options,
                         std::span<const LaunchOptionReader> readers = {});

/**
 * @returns the arguments the running process was launched with, without its
 *          name, the first of @p arguments. The views point into the strings
 *          of @p arguments, which must outlive them.
 */
[[nodiscard]] std::vector<std::string_view> processArguments(
    int argumentCount, const char* const* arguments);

/**
 * @returns the first lines of a usage: `Usage: <program> [options]`, an empty
 *          line, then `Options:`.
 */
[[nodiscard]] std::string usageHeader(std::string_view programName);

/**
 * @returns the lines of one option in a usage: @p option on a line, such as
 *          `-p <port>`, then @p description indented on the next.
 */
[[nodiscard]] std::string usageEntry(std::string_view option,
                                     std::string_view description);

/**
 * @returns what a program prints when it cannot read its arguments: its name
 *          and @p message on one line, then how to see the usage.
 */
[[nodiscard]] std::string launchErrorText(std::string_view programName,
                                          std::string_view message);

}  // namespace rtype::engine

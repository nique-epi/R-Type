#pragma once

#include <span>
#include <string>
#include <string_view>
#include "LaunchOption.hpp"

namespace rtype::engine {

/**
 * @brief Reads @p arguments in order and calls the record of the option each
 * one names, with the argument after it when the option takes a value.
 *
 * The argument after an option that takes a value is always its value, even
 * when it looks like an option. An option given twice is recorded twice, in
 * order, so the last one wins unless its record decides otherwise.
 *
 * @param arguments the arguments a program was launched with, without its
 *        name; usually what logging::LogLaunchOptions left.
 * @param options every option the program accepts.
 * @throws UnknownLaunchArgumentException for an argument that names none of
 *         @p options, a value given without its option included.
 * @throws MissingLaunchOptionValueException when an option that takes a
 *         value is the last argument.
 * @throws whatever a record throws to refuse a value.
 */
void readLaunchArguments(std::span<const std::string_view> arguments,
                         std::span<const LaunchOption> options);

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

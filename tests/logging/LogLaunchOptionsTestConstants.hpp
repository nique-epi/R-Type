#pragma once

#include <string_view>

namespace rtype::logging::testing {

/** @brief The default journal of a test that gives another one, or none. */
constexpr std::string_view UNUSED_JOURNAL = "unused.log";

/** @brief A journal in a folder that does not exist, so it cannot be opened. */
constexpr std::string_view MISSING_JOURNAL =
    "rtype_missing_directory/journal.log";

/** @brief The levels the tests give after `--log-level`, besides
 * DEBUG_LEVEL_ARGUMENT, and a word that names no level. */
constexpr std::string_view INFO_LEVEL_ARGUMENT = "info";
constexpr std::string_view ERROR_LEVEL_ARGUMENT = "error";
constexpr std::string_view NOT_A_LEVEL_ARGUMENT = "verbose";

/** @brief A logging option that does not exist, under the `--no-log-` prefix.
 */
constexpr std::string_view NO_LEVEL_OPTION = "--no-log-level";

/** @brief The module the tests log under, and the lines they log. */
constexpr std::string_view LAUNCH_LOGGER_NAME = "Launch";
constexpr std::string_view HELLO_LINE = "hello";
constexpr std::string_view SHOWN_LINE = "shown";
constexpr std::string_view HIDDEN_LINE = "hidden";
constexpr std::string_view LOST_LINE = "lost";
constexpr std::string_view KEPT_LINE = "kept";
constexpr std::string_view STILL_ON_STDERR_LINE = "still on stderr";

/** @brief How the first line of a journal starts, and what it says when there
 * is no journal. */
constexpr std::string_view JOURNAL_LINE_START = "journal: ";
constexpr std::string_view NO_JOURNAL_LINE = "no journal file";

/** @brief What starts the line of each option in the usage. */
constexpr std::string_view OPTION_LINE_START = "\n  ";

/** @brief What the usage says of the default journal of the server. */
constexpr std::string_view DEFAULT_SERVER_JOURNAL_TEXT =
    "Default: r-type_server.log next to the executable.";

}  // namespace rtype::logging::testing

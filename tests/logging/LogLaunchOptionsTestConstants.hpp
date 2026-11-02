#pragma once

#include <string_view>

namespace rtype::logging::testing {

/** @brief The default journal of a test that gives another one, or none. */
constexpr std::string_view UNUSED_JOURNAL = "unused.log";

/** @brief The journal a test gives by a path relative to the current folder. */
constexpr std::string_view RELATIVE_JOURNAL =
    "LogLaunchOptionsTest_relative_journal.log";

/** @brief A journal in a folder that does not exist, so it cannot be opened. */
constexpr std::string_view MISSING_JOURNAL_FOLDER = "rtype_missing_directory";
constexpr std::string_view MISSING_JOURNAL_FILE = "journal.log";

}  // namespace rtype::logging::testing

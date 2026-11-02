#pragma once

#include <string_view>

namespace rtype::logging::testing {

/** @brief The default journal of a test that gives another one, or none. */
constexpr std::string_view UNUSED_JOURNAL = "unused.log";

/** @brief A journal in a folder that does not exist, so it cannot be opened. */
constexpr std::string_view MISSING_JOURNAL =
    "rtype_missing_directory/journal.log";

}  // namespace rtype::logging::testing

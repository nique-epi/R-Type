#pragma once

#include <cstddef>

namespace rtype::server::testing {

/** @brief The players of the two reports the tests publish. */
constexpr std::size_t PLAYERS_OF_THE_OLDER_REPORT = 2;
constexpr std::size_t PLAYERS_OF_THE_NEWER_REPORT = 3;

/** @brief How many reports the writer thread of a test publishes, and how many
 * times the reader reads meanwhile. */
constexpr std::size_t REPORTS_PUBLISHED = 20000;
constexpr std::size_t TIMES_READ = 20000;

/** @brief The most system times a report of those tests holds, so copying one
 * really reads a heap block a publication can free. */
constexpr std::size_t MOST_SYSTEM_TIMES = 16;

}  // namespace rtype::server::testing

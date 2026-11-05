#pragma once

#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <optional>
#include <string_view>
#include <type_traits>
#include <vector>
#include "PageRequest.hpp"
#include "ServerStatus.hpp"

namespace rtype::master {

/** @brief Binds the limit and the offset of @p page to the parameters
 * LIMIT_PARAMETER and OFFSET_PARAMETER of @p statement. */
void bindPage(SQLite::Statement& statement, const PageRequest& page);

/** @brief Binds @p measure to @p parameter of @p statement, null when it was
 * not taken. */
void bindMeasure(SQLite::Statement& statement, std::string_view parameter,
                 std::optional<double> measure);

/** @returns The measure @p column holds, empty while it is null. */
[[nodiscard]] std::optional<double> measureIn(const SQLite::Column& column);

/** @returns The date @p column holds in seconds since 1970 UTC, none while it
 * is null. */
[[nodiscard]] std::optional<std::chrono::sys_seconds> optionalDateIn(
    const SQLite::Column& column);

/**
 * @returns The status @p column holds. A text that is no status of this
 * master, which it never writes, reads as Offline, and the next refresh of the
 * statuses replaces it.
 */
[[nodiscard]] network::ServerStatus statusIn(const SQLite::Column& column);

/** @brief Runs @p statement, a DELETE whose OLDEST_PARAMETER takes
 * @p oldest, on @p connection. */
void eraseOlderThan(SQLite::Database& connection, std::string_view statement,
                    std::chrono::sys_seconds oldest);

/** @returns What @p read gives of each row @p query steps through, in order. */
template <typename Read>
auto rowsOf(SQLite::Statement& query, Read read) {
  std::vector<std::invoke_result_t<Read, const SQLite::Statement&>> rows;
  while (query.executeStep()) {
    rows.push_back(read(query));
  }
  return rows;
}

}  // namespace rtype::master

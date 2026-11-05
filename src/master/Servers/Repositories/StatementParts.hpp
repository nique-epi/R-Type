#pragma once

#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Statement.h>
#include <optional>
#include <type_traits>
#include <vector>
#include "PageRequest.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerStatus.hpp"

namespace rtype::master {

/** @brief Binds the limit and the offset of @p page to the parameters
 * LIMIT_PARAMETER and OFFSET_PARAMETER of @p statement. */
void bindPage(SQLite::Statement& statement, const PageRequest& page);

/**
 * @returns The status @p column holds. A text that is no status of this
 * master, which it never writes, reads as Offline, and the next refresh of the
 * statuses replaces it.
 */
[[nodiscard]] network::ServerStatus statusIn(const SQLite::Column& column);

/**
 * @returns The heartbeat @p column holds, or nothing while it is null or holds
 * a text this master no longer reads, kept before a change of the message: the
 * next heartbeat of its server replaces it within MASTER_HEARTBEAT_INTERVAL.
 */
[[nodiscard]] std::optional<network::ServerHeartbeat> readableHeartbeatIn(
    const SQLite::Column& column);

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

#pragma once

#include <string_view>

namespace rtype::server::testing {

/** @brief Counts the tables named bans in a file: 1 while it holds the table
 * of the bans, 0 once it is dropped. */
constexpr std::string_view COUNT_BANS_TABLES =
    "SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name = 'bans'";

/** @brief A ban of ACE for good, as the console of the third version of the
 * file wrote it. */
constexpr std::string_view INSERT_BAN_OF_ACE =
    "INSERT INTO bans (kind, target, banned_at) VALUES ('nickname', 'ACE', "
    "1800000000)";

}  // namespace rtype::server::testing

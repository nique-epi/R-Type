#pragma once

#include <SQLiteCpp/Statement.h>
#include "User.hpp"

namespace rtype::master {

/**
 * @returns The account of the row @p query stands on: its id, its nickname and
 * its role in the first three columns. A role not in
 * network::USER_ROLE_TEXTS, which only an edit of the file can write, reads
 * as a player.
 */
[[nodiscard]] network::User userIn(const SQLite::Statement& query);

}  // namespace rtype::master

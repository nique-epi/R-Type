#pragma once

#include <SQLiteCpp/Statement.h>
#include "Account.hpp"

namespace rtype::master {

/**
 * @returns The account of the row @p query stands on: its id, its nickname and
 * its role in the first three columns. A role not in
 * network::ACCOUNT_ROLE_TEXTS, which only an edit of the file can write, reads
 * as a player.
 */
[[nodiscard]] network::Account accountIn(const SQLite::Statement& query);

}  // namespace rtype::master

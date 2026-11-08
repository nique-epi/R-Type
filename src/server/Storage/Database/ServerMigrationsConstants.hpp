#pragma once

#include <array>
#include <string_view>
#include "ResultsDatabaseConstants.hpp"

namespace rtype::server {

/** @brief The versions of the file of a server, in order, for every process
 * that opens it, the supervisor and each game: a change adds one, never edits
 * one, and a process with a shorter list refuses the file (see
 * storage::openDatabaseFile()). */
constexpr std::array<std::string_view, 2> SERVER_MIGRATIONS{
    RESULTS_TABLE_MIGRATION, UNSENT_GAMES_MIGRATION};

}  // namespace rtype::server

#pragma once

#include <array>
#include <string_view>

namespace rtype::master {

/**
 * @brief The migrations of the database of the master, in order:
 * MASTER_MIGRATIONS[0] is version 1. None yet: the first table, the list of
 * the game servers, comes with their registration. A change adds a migration
 * at the end and never edits one a master already applied.
 */
constexpr std::array<std::string_view, 0> MASTER_MIGRATIONS{};

}  // namespace rtype::master

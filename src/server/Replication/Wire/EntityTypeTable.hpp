#pragma once

#include <vector>
#include "EntitySheets.hpp"
#include "EntityTypeEntry.hpp"

namespace rtype::server {

/**
 * @returns The table of @p types as ConnectionAccepted carries it: one entry
 * per sheet, in the order of their numbers, which is the order of their names,
 * each with its side and its name, so entry n - 1 is the sheet numbered n.
 */
[[nodiscard]] std::vector<network::EntityTypeEntry> entityTypeTableOf(
    const game::EntitySheets& types);

}  // namespace rtype::server

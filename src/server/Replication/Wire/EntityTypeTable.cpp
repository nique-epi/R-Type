#include "EntityTypeTable.hpp"
#include <vector>
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySideMapping.hpp"
#include "EntityTypeEntry.hpp"

namespace rtype::server {

std::vector<network::EntityTypeEntry> entityTypeTableOf(
    const game::EntitySheets& types) {
  std::vector<network::EntityTypeEntry> table;
  for (const game::EntitySheet& sheet : types.all()) {
    table.push_back(network::EntityTypeEntry{.side = toEntitySide(sheet.side),
                                             .name = sheet.name});
  }
  return table;
}

}  // namespace rtype::server

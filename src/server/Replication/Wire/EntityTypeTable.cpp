#include "EntityTypeTable.hpp"
#include <vector>
#include "EntitySheet.hpp"
#include "EntitySheetConstants.hpp"
#include "EntitySheets.hpp"
#include "EntitySideMapping.hpp"
#include "EntityTypeEntry.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

static_assert(game::MAXIMUM_ENTITY_TYPES == network::MAXIMUM_ENTITY_TYPES,
              "every sheet the server loads has a place in the table");
static_assert(game::MAXIMUM_ENTITY_TYPE_NAME_LENGTH ==
                  network::MAXIMUM_ENTITY_TYPE_NAME_LENGTH,
              "the name of every sheet fits the table");

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

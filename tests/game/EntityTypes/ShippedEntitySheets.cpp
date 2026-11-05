#include "ShippedEntitySheets.hpp"
#include <filesystem>
#include "EntitySheetConstants.hpp"
#include "EntitySheets.hpp"

namespace rtype::game::testing {

std::filesystem::path shippedSheetFolder() {
  return std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER} /
         ENTITY_SHEET_FOLDER_IN_ASSETS;
}

const EntitySheets& shippedEntitySheets() {
  static const EntitySheets sheets =
      EntitySheets::fromFolder(shippedSheetFolder());
  return sheets;
}

}  // namespace rtype::game::testing

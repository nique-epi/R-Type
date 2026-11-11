#include "EntitySheetsFixtures.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "EntitySheet.hpp"
#include "EntitySheetCollisionTestConstants.hpp"
#include "EntitySheets.hpp"
#include "EntitySheetsTestConstants.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"

namespace rtype::game::testing {

std::vector<SheetText> enemyMissileSheets(std::size_t count) {
  std::vector<SheetText> sheets;
  sheets.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    sheets.push_back(
        sheetText(std::string{GENERATED_SHEET_PREFIX} + std::to_string(i),
                  ENEMY_MISSILE_SHEET_TEXT));
  }
  return sheets;
}

std::vector<SheetText> sheetsOfDistinctLayers(std::size_t count) {
  std::vector<SheetText> sheets;
  sheets.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    const std::string name =
        std::string{GENERATED_LAYER_PREFIX} + std::to_string(i);
    sheets.push_back(sheetText(name, std::string{SHEET_UP_TO_ITS_LAYER} + name +
                                         std::string{SHEET_FROM_ITS_PILOT}));
  }
  return sheets;
}

std::vector<std::string> namesIn(const EntitySheets& sheets) {
  std::vector<std::string> names;
  for (const EntitySheet& sheet : sheets.all()) {
    names.push_back(sheet.name);
  }
  return names;
}

}  // namespace rtype::game::testing

#include "EntitySheetsFixtures.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "EntitySheet.hpp"
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

std::vector<std::string> namesIn(const EntitySheets& sheets) {
  std::vector<std::string> names;
  for (const EntitySheet& sheet : sheets.all()) {
    names.push_back(sheet.name);
  }
  return names;
}

}  // namespace rtype::game::testing

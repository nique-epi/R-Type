#include "EntitySheetsFixtures.hpp"
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include "DataFileConstants.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySheetsTestConstants.hpp"
#include "SheetText.hpp"

namespace rtype::game::testing {

SheetText sheetText(std::string_view name, std::string_view text) {
  return SheetText{
      .name = std::string{name},
      .source = std::string{name} + std::string{engine::DATA_FILE_EXTENSION},
      .text = std::string{text}};
}

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

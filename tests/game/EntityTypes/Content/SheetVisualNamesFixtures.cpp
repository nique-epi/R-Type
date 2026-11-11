#include "SheetVisualNamesFixtures.hpp"
#include <string_view>
#include <toml++/toml.hpp>
#include "DataFileReading.hpp"
#include "EntitySheetConstants.hpp"
#include "SheetText.hpp"
#include "SheetTextFixtures.hpp"
#include "SheetTextReading.hpp"
#include "SheetVisualNames.hpp"
#include "SheetVisualNamesTestConstants.hpp"

namespace rtype::game::testing {

SheetVisualNames visualNamesIn(std::string_view text) {
  const SheetText sheet = sheetText(SHEET_NAME, text);
  const toml::table root = engine::parseDataFile(sheet.text, sheet.source);
  return visualNamesOf(
      sheetTableOf(engine::rootOf(root, sheet.source), APPEARANCE_KEY));
}

}  // namespace rtype::game::testing

#include "EntityAppearancesFixtures.hpp"
#include <array>
#include <string_view>
#include "EntityAppearances.hpp"
#include "EntityAppearancesTestConstants.hpp"
#include "EntityTypesTestConstants.hpp"
#include "SheetTextFixtures.hpp"
#include "ShippedEntitySheets.hpp"

using rtype::client::EntityAppearances;
using rtype::game::testing::DRIFTER_SHEET;
using rtype::game::testing::sheetText;
using rtype::game::testing::shippedSheetFolder;

EntityAppearances appearancesReading(std::string_view text) {
  const std::array sheets{sheetText(READ_TYPE_NAME, text)};
  return EntityAppearances::fromTexts(sheets);
}

EntityAppearances threeAppearancesOnTwoSheets() {
  const std::array sheets{sheetText(READ_TYPE_NAME, READ_SHEET_TEXT),
                          sheetText(UNREAD_TYPE_NAME, OTHER_SHEET_TEXT),
                          sheetText(DRIFTER_SHEET, READ_SHEET_TEXT)};
  return EntityAppearances::fromTexts(sheets);
}

EntityAppearances shippedAppearances() {
  return EntityAppearances::fromFolder(shippedSheetFolder());
}

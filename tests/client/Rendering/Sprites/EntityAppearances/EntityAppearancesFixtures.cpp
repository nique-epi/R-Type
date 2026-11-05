#include "EntityAppearancesFixtures.hpp"
#include <array>
#include <filesystem>
#include <string>
#include <string_view>
#include "EntityAppearances.hpp"
#include "EntityAppearancesTestConstants.hpp"
#include "SheetText.hpp"

using rtype::client::EntityAppearances;
using rtype::game::SheetText;

namespace {

/** @returns The folder of the sheets shipped in the assets of the sources. */
std::filesystem::path shippedSheetFolder() {
  return std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER} /
         SHIPPED_SHEET_FOLDER;
}

}  // namespace

EntityAppearances appearancesReading(std::string_view text) {
  const std::array sheets{SheetText{.name = std::string{READ_TYPE_NAME},
                                    .source = std::string{READ_TYPE_NAME},
                                    .text = std::string{text}}};
  return EntityAppearances::fromTexts(sheets);
}

EntityAppearances threeAppearancesOnTwoSheets() {
  const std::array sheets{SheetText{.name = std::string{READ_TYPE_NAME},
                                    .source = std::string{READ_TYPE_NAME},
                                    .text = std::string{READ_SHEET_TEXT}},
                          SheetText{.name = std::string{UNREAD_TYPE_NAME},
                                    .source = std::string{UNREAD_TYPE_NAME},
                                    .text = std::string{OTHER_SHEET_TEXT}},
                          SheetText{.name = std::string{DRIFTER_TYPE_NAME},
                                    .source = std::string{DRIFTER_TYPE_NAME},
                                    .text = std::string{READ_SHEET_TEXT}}};
  return EntityAppearances::fromTexts(sheets);
}

EntityAppearances shippedAppearances() {
  return EntityAppearances::fromFolder(shippedSheetFolder());
}

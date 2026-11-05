#include "ScreenAssetsFixtures.hpp"
#include <array>
#include <filesystem>
#include <string>
#include <string_view>
#include "EntityAppearances.hpp"
#include "ScreenAssetsTestConstants.hpp"
#include "SheetTextFixtures.hpp"

std::filesystem::path repositoryAssets() { return RTYPE_ASSETS_FOLDER; }

rtype::client::EntityAppearances appearanceDrawnFrom(std::string_view sheetId) {
  const std::array sheets{rtype::game::testing::sheetText(
      NEW_ENTITY_TYPE_NAME, std::string{NEW_APPEARANCE_BEFORE_SHEET} +
                                std::string{sheetId} +
                                std::string{NEW_APPEARANCE_AFTER_SHEET})};
  return rtype::client::EntityAppearances::fromTexts(sheets);
}

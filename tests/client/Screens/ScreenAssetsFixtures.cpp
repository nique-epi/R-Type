#include "ScreenAssetsFixtures.hpp"
#include <array>
#include <filesystem>
#include <string>
#include <string_view>
#include "EntityAppearances.hpp"
#include "ScreenAssetsTestConstants.hpp"
#include "SheetText.hpp"

std::filesystem::path repositoryAssets() { return RTYPE_ASSETS_FOLDER; }

rtype::client::EntityAppearances appearanceDrawnFrom(std::string_view sheetId) {
  const std::array sheets{rtype::game::SheetText{
      .name = std::string{NEW_ENTITY_TYPE_NAME},
      .source = std::string{NEW_ENTITY_TYPE_NAME},
      .text = std::string{NEW_APPEARANCE_BEFORE_SHEET} + std::string{sheetId} +
              std::string{NEW_APPEARANCE_AFTER_SHEET}}};
  return rtype::client::EntityAppearances::fromTexts(sheets);
}

#include "ScreenAssets.hpp"
#include <algorithm>
#include <string>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadingStep.hpp"
#include "EffectConstants.hpp"
#include "EntityAppearances.hpp"
#include "EntitySpriteConstants.hpp"
#include "FlightLogSheetConstants.hpp"
#include "HudSpriteConstants.hpp"
#include "InterfaceConstants.hpp"
#include "LeaderboardSpriteConstants.hpp"
#include "ProfessorSpriteConstants.hpp"
#include "ScreenConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Loads every asset of a list at once, after releasing the loaded
 * assets it does not name, except the permanent ones. */
void loadAtOnce(AssetLibrary& library, const AssetList& assets) {
  AssetLoadingStep loading(library, assets);
  while (!loading.isFinished()) {
    loading.loadNext();
  }
}

}  // namespace

AssetList interfaceAssets() {
  return AssetList{.fonts = {REGULAR_FONT_ID, SMALL_FONT_ID}};
}

AssetList titleAssets() {
  return AssetList{.textures = {LOGO_ID, ENTITY_SHEET_ID, HUD_SHEET_ID,
                                PROFESSOR_SHEET_ID, LEADERBOARD_SHEET_ID}};
}

AssetList gameAssets() {
  AssetList assets = titleAssets();
  assets.textures.emplace_back(EFFECT_SHEET_ID);
  assets.textures.emplace_back(PLUG_ICON_ID);
  assets.textures.emplace_back(FLIGHT_LOG_SHEET_ID);
  return assets;
}

AssetList withEntitySheets(AssetList assets,
                           const EntityAppearances& appearances) {
  for (const std::string& sheet : appearances.sheets()) {
    if (std::ranges::find(assets.textures, sheet) == assets.textures.end()) {
      assets.textures.push_back(sheet);
    }
  }
  return assets;
}

void loadInterfaceAssets(AssetLibrary& library) {
  loadAtOnce(library, interfaceAssets());
  library.keepPermanently(interfaceAssets());
}

AssetList introAssets() {
  AssetList assets = titleAssets();
  assets.textures.emplace_back(EFFECT_SHEET_ID);
  return assets;
}

void loadIntroAssets(AssetLibrary& library) {
  loadAtOnce(library, introAssets());
}

}  // namespace rtype::client

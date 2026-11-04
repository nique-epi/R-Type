#include "ScreenAssets.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadingStep.hpp"
#include "EffectConstants.hpp"
#include "EnemyAppearance.hpp"
#include "EnemyKind.hpp"
#include "EntitySpriteConstants.hpp"
#include "FlightLogSheetConstants.hpp"
#include "HudSpriteConstants.hpp"
#include "InterfaceConstants.hpp"
#include "LeaderboardSpriteConstants.hpp"
#include "ProfessorSpriteConstants.hpp"
#include "ScreenConstants.hpp"
#include "SoundConstants.hpp"

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

/** @brief Adds the sheet of every kind of enemy to @p assets, once each. */
void addEnemySheets(AssetList& assets) {
  for (const game::EnemyKind kind : game::ALL_ENEMY_KINDS) {
    const std::string sheet = appearanceOf(kind).sheetId;
    if (std::ranges::find(assets.textures, sheet) == assets.textures.end()) {
      assets.textures.push_back(sheet);
    }
  }
}

}  // namespace

AssetList interfaceAssets() {
  return AssetList{.sounds = std::vector<std::string>(ALL_SOUND_IDS.begin(),
                                                      ALL_SOUND_IDS.end()),
                   .fonts = {REGULAR_FONT_ID, SMALL_FONT_ID}};
}

AssetList titleAssets() {
  return AssetList{.textures = {LOGO_ID, ENTITY_SHEET_ID, HUD_SHEET_ID,
                                PROFESSOR_SHEET_ID, LEADERBOARD_SHEET_ID}};
}

AssetList gameAssets() {
  AssetList assets = titleAssets();
  addEnemySheets(assets);
  assets.textures.emplace_back(EFFECT_SHEET_ID);
  assets.textures.emplace_back(PLUG_ICON_ID);
  assets.textures.emplace_back(FLIGHT_LOG_SHEET_ID);
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

#include "ScreenAssets.hpp"
#include <string>
#include <vector>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadingStep.hpp"
#include "EffectConstants.hpp"
#include "EntitySpriteConstants.hpp"
#include "FlightLogSheetConstants.hpp"
#include "HudSpriteConstants.hpp"
#include "InterfaceConstants.hpp"
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

}  // namespace

AssetList interfaceAssets() {
  return AssetList{.sounds = std::vector<std::string>(ALL_SOUND_IDS.begin(),
                                                      ALL_SOUND_IDS.end()),
                   .fonts = {REGULAR_FONT_ID, SMALL_FONT_ID}};
}

AssetList titleAssets() {
  return AssetList{
      .textures = {LOGO_ID, ENTITY_SHEET_ID, HUD_SHEET_ID, PROFESSOR_SHEET_ID}};
}

AssetList gameAssets() {
  return AssetList{.textures = {ENTITY_SHEET_ID, EFFECT_SHEET_ID, HUD_SHEET_ID,
                                PLUG_ICON_ID, FLIGHT_LOG_SHEET_ID,
                                PROFESSOR_SHEET_ID}};
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

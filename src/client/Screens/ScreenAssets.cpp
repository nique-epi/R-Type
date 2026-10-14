#include "ScreenAssets.hpp"
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadingStep.hpp"
#include "EntitySpriteConstants.hpp"
#include "InterfaceConstants.hpp"
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
  return AssetList{
      .fonts = {INTERFACE_FONT_ID, REGULAR_FONT_ID, SMALL_FONT_ID}};
}

AssetList titleAssets() {
  return AssetList{.textures = {LOGO_ID, ENTITY_SHEET_ID}};
}

AssetList gameAssets() { return AssetList{.textures = {ENTITY_SHEET_ID}}; }

void loadInterfaceAssets(AssetLibrary& library) {
  loadAtOnce(library, interfaceAssets());
  library.keepPermanently(interfaceAssets());
}

void loadTitleAssets(AssetLibrary& library) {
  loadAtOnce(library, titleAssets());
}

}  // namespace rtype::client

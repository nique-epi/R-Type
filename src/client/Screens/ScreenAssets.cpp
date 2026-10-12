#include "ScreenAssets.hpp"
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadingStep.hpp"
#include "EntitySpriteConstants.hpp"
#include "ScreenConstants.hpp"

namespace rtype::client {

AssetList interfaceAssets() { return AssetList{.fonts = {INTERFACE_FONT_ID}}; }

AssetList gameAssets() { return AssetList{.textures = {ENTITY_SHEET_ID}}; }

void loadInterfaceAssets(AssetLibrary& library) {
  AssetLoadingStep loading(library, interfaceAssets());
  while (!loading.isFinished()) {
    loading.loadNext();
  }
  library.keepPermanently(interfaceAssets());
}

}  // namespace rtype::client

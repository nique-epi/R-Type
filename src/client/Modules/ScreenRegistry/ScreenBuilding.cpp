#include "ScreenBuilding.hpp"
#include <SFML/System/Vector2.hpp>
#include <memory>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "GameWindow.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "InterfaceCanvas.hpp"
#include "LoadingScreen.hpp"
#include "LoadingScreenParts.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "ReducedMotion.hpp"
#include "ScreenContext.hpp"
#include "ScreenId.hpp"

namespace rtype::client {

MotionPreference motionPreference() {
  return systemPrefersReducedMotion() ? MotionPreference::Reduced
                                      : MotionPreference::Full;
}

std::unique_ptr<IInterfaceCanvas> interfaceCanvasOf(
    const ScreenContext& context) {
  return std::make_unique<InterfaceCanvas>(context.services.assets.get());
}

PlayfieldMapping playfieldMappingOf(const ScreenContext& context) {
  const GameWindow& window = context.services.window.get();
  return
      [&window](sf::Vector2i pixel) { return window.playfieldPointAt(pixel); };
}

std::unique_ptr<IScreen> loadingScreenUnlessLoaded(const ScreenContext& context,
                                                   const AssetList& assets,
                                                   ScreenId screen) {
  AssetLibrary& library = context.services.assets.get();
  if (!library.isLoaded(assets)) {
    return std::make_unique<LoadingScreen>(
        LoadingScreenParts{.navigator = context.navigator.get(),
                           .library = library,
                           .assets = assets,
                           .nextScreen = screen});
  }
  library.releaseAllExcept(assets);
  return nullptr;
}

}  // namespace rtype::client

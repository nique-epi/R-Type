#include "ScreenBuilding.hpp"
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <utility>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "DialogScreen.hpp"
#include "GameWindow.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "ISoundPlayer.hpp"
#include "InterfaceCanvas.hpp"
#include "InterfaceLook.hpp"
#include "LoadingScreen.hpp"
#include "LoadingScreenParts.hpp"
#include "Menu.hpp"
#include "MotionPreference.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldMapping.hpp"
#include "ReducedMotion.hpp"
#include "ScreenContext.hpp"
#include "ScreenId.hpp"
#include "TextMeasures.hpp"

namespace rtype::client {

MotionPreference motionPreference() {
  return systemPrefersReducedMotion() ? MotionPreference::Reduced
                                      : MotionPreference::Full;
}

ISoundPlayer& soundsOf(const ScreenContext& context) {
  return context.services.audio.get().sounds();
}

InterfaceLook interfaceLookOf(const PlayerSettings& settings) {
  return InterfaceLook{.text = textMetricsOf(settings.textSize),
                       .highContrast = settings.highContrast};
}

std::unique_ptr<IInterfaceCanvas> interfaceCanvasOf(
    const ScreenContext& context) {
  const PlayerSettings& settings = context.services.settings.get();
  return std::make_unique<InterfaceCanvas>(
      context.services.assets.get(),
      [&settings] { return interfaceLookOf(settings); });
}

PlayfieldMapping playfieldMappingOf(const ScreenContext& context) {
  const GameWindow& window = context.services.window.get();
  return
      [&window](sf::Vector2i pixel) { return window.playfieldPointAt(pixel); };
}

std::unique_ptr<IScreen> dialogScreenOf(const ScreenContext& context,
                                        Menu menu) {
  return std::make_unique<DialogScreen>(
      std::move(menu), playfieldMappingOf(context), interfaceCanvasOf(context));
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

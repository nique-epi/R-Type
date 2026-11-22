#include "ScreenBuilding.hpp"
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "DialogScreen.hpp"
#include "GameWindow.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "InterfaceCanvas.hpp"
#include "Language.hpp"
#include "LoadingScreen.hpp"
#include "LoadingScreenParts.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "ReducedMotion.hpp"
#include "ScreenConstants.hpp"
#include "ScreenContext.hpp"
#include "ScreenId.hpp"
#include "ScreenRegistry.hpp"
#include "SignInReturn.hpp"
#include "SignInScreenIdConstants.hpp"

namespace rtype::client {

MotionPreference motionPreference() {
  return systemPrefersReducedMotion() ? MotionPreference::Reduced
                                      : MotionPreference::Full;
}

ISoundPlayer& soundsOf(const ScreenContext& context) {
  return context.services.audio.get().sounds();
}

std::unique_ptr<IInterfaceCanvas> interfaceCanvasOf(
    const ScreenContext& context) {
  return std::make_unique<InterfaceCanvas>(
      context.services.assets.get(),
      [&settings = context.services.settings.get()] {
        return settings.language;
      });
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

std::unique_ptr<IScreen> signInNeededScreenOf(const ScreenContext& context,
                                              std::optional<ScreenId> asking) {
  IScreenNavigator& navigator = context.navigator.get();
  SignInReturn& signInReturn = context.services.signInReturn.get();
  const Language language = context.services.settings.get().language;
  return dialogScreenOf(
      context, MenuBuilder(MenuBuilder::Frame::Dialog, &soundsOf(context))
                   .heading(std::string(SIGN_IN_HEADING.in(language)))
                   .message(std::string(SIGN_IN_MESSAGE.in(language)))
                   .button({.label = std::string(SIGN_IN_LABEL.in(language))},
                           [&navigator, &signInReturn, asking] {
                             signInReturn = SignInReturn{.closesAsking = true,
                                                         .screen = asking};
                             navigator.push(SIGN_IN_SCREEN);
                           })
                   .cancelButton({.label = std::string(OK_LABEL.in(language))},
                                 [&navigator] { navigator.pop(); })
                   .build());
}

ScreenRegistry::Builder signedInOnly(ScreenRegistry::Builder builder,
                                     ScreenId asking) {
  return [builder = std::move(builder), asking](const ScreenContext& context) {
    if (!context.services.account.get().signedIn()) {
      return signInNeededScreenOf(context, asking);
    }
    return builder(context);
  };
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

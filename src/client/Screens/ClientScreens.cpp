#include "ClientScreens.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <array>
#include <memory>
#include <random>
#include <string>
#include <string_view>
#include <utility>
#include "AssetKind.hpp"
#include "AssetLibrary.hpp"
#include "BackgroundCatalog.hpp"
#include "ISessionBackgrounds.hpp"
#include "MenuScreenIdConstants.hpp"
#include "OpeningScreen.hpp"
#include "ScreenAssets.hpp"
#include "ScreenContext.hpp"
#include "ScreenId.hpp"
#include "ScreenRegistry.hpp"
#include "ScreenServices.hpp"
#include "ServerChoice.hpp"
#include "SkySources.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The backgrounds of the assets of @p library, their images
 * loaded for good. */
BackgroundCatalog loadedBackgroundsOf(AssetLibrary& library) {
  const std::array folders{library.folder()};
  BackgroundCatalog backgrounds = BackgroundCatalog::ofLastFolder(folders);
  loadBackgroundAssets(library, backgrounds);
  return backgrounds;
}

/** @returns Whether @p library holds every image @p backgrounds name; it
 * does for no background. */
bool holdsTheImagesOf(const AssetLibrary& library,
                      const BackgroundCatalog* backgrounds) {
  return backgrounds == nullptr ||
         std::ranges::all_of(
             backgrounds->images(), [&library](const std::string& image) {
               return library.isLoaded(AssetKind::Texture, image);
             });
}

}  // namespace

ClientScreens::ClientScreens(const ScreenServices& parts,
                             const ScreenRegistry& registry)
    : connection_(&parts.connection.get()),
      sessionBackgrounds_(&parts.sessionBackgrounds.get()),
      assets_(&parts.assets.get()),
      backgrounds_(loadedBackgroundsOf(parts.assets.get())),
      background_(
          backgrounds_,
          SkySources{.seed = std::random_device{}(),
                     .textureOf =
                         [assets = parts.assets](std::string_view assetId) {
                           return assets.get().findTexture(assetId);
                         },
                     .fileOf =
                         [assets = parts.assets](std::string_view assetId) {
                           return assets.get().fileOf(assetId);
                         }}),
      serverChoice_(parts.choice),
      screens_([this, &registry](ScreenId screen) {
        return registry.create(screen, context_);
      }),
      context_(ScreenContext{.services = parts,
                             .navigator = screens_,
                             .sky = background_,
                             .gameResult = gameResult_,
                             .previousTaunt = previousTaunt_,
                             .serverChoice = serverChoice_}) {
  screens_.replaceAll(openingScreen());
  screens_.applyPendingChanges();
}

void ClientScreens::handleEvent(const sf::Event& event) {
  screens_.handleEvent(event);
  screens_.applyPendingChanges();
}

void ClientScreens::update(engine::Duration elapsed) {
  followSessionBackgrounds();
  background_.advance(elapsed);
  screens_.update(elapsed);
  if (sessionLoss_.endsTheSession(connection_->status())) {
    screens_.replaceAll(TITLE_SCREEN);
    screens_.push(CONNECTION_LOST_SCREEN);
  }
  screens_.applyPendingChanges();
}

void ClientScreens::draw(sf::RenderTarget& target) {
  background_.draw(target);
  screens_.draw(target);
}

bool ClientScreens::isEmpty() const { return screens_.isEmpty(); }

void ClientScreens::followSessionBackgrounds() {
  std::shared_ptr<const BackgroundCatalog> session =
      sessionBackgrounds_->backgrounds();
  if (session.get() != background_.laidOver() &&
      holdsTheImagesOf(*assets_, session.get())) {
    background_.layOver(std::move(session));
  }
}

}  // namespace rtype::client

#include "ClientScreens.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <random>
#include <string_view>
#include "AssetLibrary.hpp"
#include "MenuScreenIdConstants.hpp"
#include "OpeningScreen.hpp"
#include "ScreenContext.hpp"
#include "ScreenId.hpp"
#include "ScreenRegistry.hpp"
#include "ScreenServices.hpp"
#include "ServerChoice.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ClientScreens::ClientScreens(const ScreenServices& parts,
                             const ScreenRegistry& registry)
    : connection_(&parts.connection.get()),
      background_(std::random_device{}(),
                  [assets = parts.assets](std::string_view assetId) {
                    return assets.get().findTexture(assetId);
                  }),
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

}  // namespace rtype::client

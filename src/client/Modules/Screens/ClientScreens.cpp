#include "ClientScreens.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <random>
#include "AudioConstants.hpp"
#include "ClientScreensParts.hpp"
#include "ScreenCatalog.hpp"
#include "ScreenCatalogParts.hpp"
#include "ScreenId.hpp"
#include "SoundtrackParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ClientScreens::ClientScreens(const ClientScreensParts& parts)
    : settingsService_(&parts.settingsService.get()),
      background_(std::random_device{}()),
      sounds_(parts.assets.get(), *settingsService_),
      theme_(parts.assets.get(), *settingsService_, THEME_TRACK),
      gameMusic_(parts.assets.get(), *settingsService_, GAME_TRACK),
      soundtrack_(SoundtrackParts{.theme = theme_, .gameMusic = gameMusic_}),
      screens_([this](ScreenId screen) { return catalog_.create(screen); }),
      catalog_(ScreenCatalogParts{.services = parts,
                                  .navigator = screens_,
                                  .sky = background_,
                                  .sounds = sounds_,
                                  .theme = theme_,
                                  .soundtrack = soundtrack_,
                                  .gameResult = gameResult_,
                                  .previousTaunt = previousTaunt_}) {
  screens_.replaceAll(ScreenCatalog::firstScreen());
  screens_.applyPendingChanges();
  settingsService_->subscribe(sounds_);
  settingsService_->subscribe(theme_);
  settingsService_->subscribe(gameMusic_);
}

void ClientScreens::handleEvent(const sf::Event& event) {
  screens_.handleEvent(event);
  screens_.applyPendingChanges();
}

void ClientScreens::update(engine::Duration elapsed) {
  soundtrack_.advance(elapsed);
  background_.advance(elapsed);
  screens_.update(elapsed);
  screens_.applyPendingChanges();
}

void ClientScreens::draw(sf::RenderTarget& target) {
  background_.draw(target);
  screens_.draw(target);
}

bool ClientScreens::isEmpty() const { return screens_.isEmpty(); }

void ClientScreens::stopListening() {
  settingsService_->unsubscribe(gameMusic_);
  settingsService_->unsubscribe(theme_);
  settingsService_->unsubscribe(sounds_);
}

}  // namespace rtype::client

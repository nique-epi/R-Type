#include "ClientScreens.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <random>
#include "AudioConstants.hpp"
#include "OpeningScreen.hpp"
#include "ScreenContext.hpp"
#include "ScreenId.hpp"
#include "ScreenRegistry.hpp"
#include "ScreenServices.hpp"
#include "SoundtrackParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ClientScreens::ClientScreens(const ScreenServices& parts,
                             const ScreenRegistry& registry)
    : background_(std::random_device{}()),
      sounds_(parts.assets.get(), parts.settings.get()),
      theme_(parts.assets.get(), parts.settings.get(), THEME_TRACK),
      gameMusic_(parts.assets.get(), parts.settings.get(), GAME_TRACK),
      soundtrack_(SoundtrackParts{.theme = theme_, .gameMusic = gameMusic_}),
      screens_([this, &registry](ScreenId screen) {
        return registry.create(screen, context_);
      }),
      context_(ScreenContext{.services = parts,
                             .navigator = screens_,
                             .sky = background_,
                             .sounds = sounds_,
                             .theme = theme_,
                             .soundtrack = soundtrack_,
                             .gameResult = gameResult_,
                             .previousTaunt = previousTaunt_}) {
  screens_.replaceAll(openingScreen());
  screens_.applyPendingChanges();
}

void ClientScreens::handleEvent(const sf::Event& event) {
  screens_.handleEvent(event);
  screens_.applyPendingChanges();
}

void ClientScreens::update(engine::Duration elapsed) {
  theme_.update();
  gameMusic_.update();
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

}  // namespace rtype::client

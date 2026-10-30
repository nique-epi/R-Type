#include "ClientScreens.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <random>
#include "OpeningScreen.hpp"
#include "ScreenContext.hpp"
#include "ScreenId.hpp"
#include "ScreenRegistry.hpp"
#include "ScreenServices.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ClientScreens::ClientScreens(const ScreenServices& parts,
                             const ScreenRegistry& registry)
    : background_(std::random_device{}()),
      screens_([this, &registry](ScreenId screen) {
        return registry.create(screen, context_);
      }),
      context_(ScreenContext{.services = parts,
                             .navigator = screens_,
                             .sky = background_,
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

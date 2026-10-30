#include "ClientScreens.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <random>
#include "ClientScreensParts.hpp"
#include "ScreenCatalog.hpp"
#include "ScreenCatalogParts.hpp"
#include "ScreenId.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ClientScreens::ClientScreens(const ClientScreensParts& parts)
    : background_(std::random_device{}()),
      screens_([this](ScreenId screen) { return catalog_.create(screen); }),
      catalog_(ScreenCatalogParts{.services = parts,
                                  .navigator = screens_,
                                  .sky = background_,
                                  .gameResult = gameResult_,
                                  .previousTaunt = previousTaunt_}) {
  screens_.replaceAll(ScreenCatalog::firstScreen());
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

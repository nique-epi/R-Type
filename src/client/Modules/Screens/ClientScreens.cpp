#include "ClientScreens.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <random>
#include "ClientScreensParts.hpp"
#include "ScreenCatalog.hpp"
#include "ScreenCatalogParts.hpp"
#include "ScreenId.hpp"
#include "ServerChoice.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ClientScreens::ClientScreens(const ClientScreensParts& parts)
    : connection_(&parts.connection.get()),
      background_(std::random_device{}()),
      serverChoice_(
          ServerChoice{.server = parts.server, .nickname = parts.nickname}),
      screens_([this](ScreenId screen) { return catalog_.create(screen); }),
      catalog_(ScreenCatalogParts{.services = parts,
                                  .navigator = screens_,
                                  .sky = background_,
                                  .gameResult = gameResult_,
                                  .previousTaunt = previousTaunt_,
                                  .serverChoice = serverChoice_}) {
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
  if (sessionLoss_.endsTheSession(connection_->status())) {
    screens_.replaceAll(ScreenId::Title);
    screens_.push(ScreenId::ConnectionLost);
  }
  screens_.applyPendingChanges();
}

void ClientScreens::draw(sf::RenderTarget& target) {
  background_.draw(target);
  screens_.draw(target);
}

bool ClientScreens::isEmpty() const { return screens_.isEmpty(); }

}  // namespace rtype::client

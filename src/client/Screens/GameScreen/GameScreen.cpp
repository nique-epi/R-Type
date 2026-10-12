#include "GameScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <utility>
#include "IEntityLayer.hpp"
#include "IScreenNavigator.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "SpriteShowcase.hpp"

namespace rtype::client {

GameScreen::GameScreen(IScreenNavigator& navigator,
                       std::unique_ptr<IEntityLayer> entityLayer)
    : navigator_(&navigator), entityLayer_(std::move(entityLayer)) {
  spawnSpriteShowcase(entities_, components_);
  entityLayer_->paint(components_);
}

void GameScreen::handleEvent(const sf::Event& event) {
  const auto* released = event.getIf<sf::Event::KeyReleased>();
  if (released != nullptr && released->code == OPTIONS_KEY) {
    navigator_->push(ScreenId::Options);
  }
}

void GameScreen::update() { entityLayer_->paint(components_); }

void GameScreen::draw(sf::RenderTarget& target) const {
  entityLayer_->show(target);
}

}  // namespace rtype::client

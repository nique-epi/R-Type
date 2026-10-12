#include "GameScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"

namespace rtype::client {

GameScreen::GameScreen(ScreenStack& screens) : screens_(&screens) {}

void GameScreen::handleEvent(const sf::Event& event) {
  const auto* released = event.getIf<sf::Event::KeyReleased>();
  if (released != nullptr && released->code == OPTIONS_KEY) {
    screens_->push(ScreenId::Options);
  }
}

void GameScreen::update() {}

void GameScreen::draw([[maybe_unused]] sf::RenderTarget& target) const {}

}  // namespace rtype::client

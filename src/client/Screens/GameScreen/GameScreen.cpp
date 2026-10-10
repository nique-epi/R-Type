#include "GameScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <utility>
#include "ScreenConstants.hpp"
#include "ScreenStack.hpp"

namespace rtype::client {

GameScreen::GameScreen(ScreenStack& screens, ScreenFactory optionsScreen)
    : screens_(&screens), optionsScreen_(std::move(optionsScreen)) {}

void GameScreen::handleEvent(const sf::Event& event) {
  const auto* released = event.getIf<sf::Event::KeyReleased>();
  if (released != nullptr && released->code == OPTIONS_KEY) {
    screens_->push(optionsScreen_);
  }
}

void GameScreen::update() {}

void GameScreen::draw([[maybe_unused]] sf::RenderTarget& target) const {}

}  // namespace rtype::client

#include "GameScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <utility>
#include "PlayfieldConstants.hpp"
#include "ScreenColors.hpp"
#include "ScreenConstants.hpp"
#include "ScreenStack.hpp"

namespace rtype::client {

GameScreen::GameScreen(ScreenStack& screens, ScreenFactory optionsScreen)
    : screens_(&screens),
      optionsScreen_(std::move(optionsScreen)),
      playfieldBackground_({game::PLAYFIELD_WIDTH, game::PLAYFIELD_HEIGHT}) {
  playfieldBackground_.setFillColor(PLAYFIELD_BACKGROUND_COLOR);
}

void GameScreen::handleEvent(const sf::Event& event) {
  const auto* pressed = event.getIf<sf::Event::KeyPressed>();
  if (pressed != nullptr && pressed->code == OPTIONS_KEY) {
    screens_->push(optionsScreen_);
  }
}

void GameScreen::update() {}

void GameScreen::draw(sf::RenderTarget& target) const {
  target.draw(playfieldBackground_);
}

}  // namespace rtype::client

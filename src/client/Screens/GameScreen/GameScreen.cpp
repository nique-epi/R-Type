#include "GameScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdint>
#include "ActionSet.hpp"
#include "IScreenNavigator.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"

namespace rtype::client {

GameScreen::GameScreen(IScreenNavigator& navigator) : navigator_(&navigator) {}

void GameScreen::handleEvent(const sf::Event& event) {
  const auto* released = event.getIf<sf::Event::KeyReleased>();
  if (released != nullptr && released->code == OPTIONS_KEY) {
    navigator_->push(ScreenId::Options);
  }
  if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
    playerCommands_.keyPressed(keyPressed->code);
  } else if (released != nullptr) {
    playerCommands_.keyReleased(released->code);
  } else if (event.is<sf::Event::FocusLost>()) {
    playerCommands_.focusLost();
  } else if (event.is<sf::Event::FocusGained>()) {
    playerCommands_.focusGained();
  }
}

void GameScreen::update() {
  const std::uint8_t held = playerCommands_.held().bits();
  if (held != loggedActions_) {
    logger_.trace("actions held: ", static_cast<unsigned int>(held));
    loggedActions_ = held;
  }
}

ActionSet GameScreen::playerActions() const { return playerCommands_.held(); }

void GameScreen::draw([[maybe_unused]] sf::RenderTarget& target) const {}

}  // namespace rtype::client

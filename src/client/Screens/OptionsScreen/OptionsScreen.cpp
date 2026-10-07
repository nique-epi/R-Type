#include "OptionsScreen.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <optional>
#include "GameWindow.hpp"
#include "PlayfieldConstants.hpp"
#include "ScreenColors.hpp"
#include "ScreenConstants.hpp"
#include "ScreenStack.hpp"

namespace rtype::client {

OptionsScreen::OptionsScreen(ScreenStack& screens, GameWindow& window,
                             const sf::Font& labelFont)
    : screens_(&screens),
      window_(&window),
      backdrop_({game::PLAYFIELD_WIDTH, game::PLAYFIELD_HEIGHT}),
      windowSizeButtons_(labelFont) {
  backdrop_.setFillColor(OPTIONS_BACKDROP_COLOR);
  windowSizeButtons_.showState(window_->windowSizeSelection());
}

void OptionsScreen::handleEvent(const sf::Event& event) {
  const auto* pressed = event.getIf<sf::Event::KeyPressed>();
  if (pressed != nullptr && pressed->code == OPTIONS_KEY) {
    screens_->pop();
    return;
  }
  const auto* clicked = event.getIf<sf::Event::MouseButtonPressed>();
  if (clicked != nullptr && clicked->button == sf::Mouse::Button::Left) {
    selectWindowSizeAt(clicked->position);
  }
}

void OptionsScreen::update() {}

void OptionsScreen::draw(sf::RenderTarget& target) const {
  target.draw(backdrop_);
  windowSizeButtons_.draw(target);
}

void OptionsScreen::selectWindowSizeAt(sf::Vector2i pixel) {
  const std::optional<std::size_t> index =
      windowSizeButtons_.indexAt(window_->playfieldPointAt(pixel));
  if (index.has_value() && window_->selectWindowSize(*index)) {
    windowSizeButtons_.showState(window_->windowSizeSelection());
  }
}

}  // namespace rtype::client

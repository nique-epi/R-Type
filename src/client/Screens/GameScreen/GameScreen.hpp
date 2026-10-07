#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include "IScreen.hpp"
#include "ScreenStack.hpp"

namespace rtype::client {

/** @brief The game in progress, which puts the options over itself when the
 * options key is released. */
class GameScreen : public IScreen {
 public:
  GameScreen(ScreenStack& screens, ScreenFactory optionsScreen);

  void handleEvent(const sf::Event& event) override;
  void update() override;
  void draw(sf::RenderTarget& target) const override;

 private:
  ScreenStack* screens_;
  ScreenFactory optionsScreen_;
  sf::RectangleShape playfieldBackground_;
};

}  // namespace rtype::client

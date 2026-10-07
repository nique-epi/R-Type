#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include "IScreen.hpp"
#include "ScreenStack.hpp"
#include "WindowSizeButtons.hpp"

namespace rtype::client {

class GameWindow;

/** @brief Options laid over the game, holding the window size buttons, closed
 * when the options key is released. */
class OptionsScreen : public IScreen {
 public:
  OptionsScreen(ScreenStack& screens, GameWindow& window,
                const sf::Font& labelFont);

  void handleEvent(const sf::Event& event) override;
  void update() override;
  void draw(sf::RenderTarget& target) const override;

 private:
  void selectWindowSizeAt(sf::Vector2i pixel);

  ScreenStack* screens_;
  GameWindow* window_;
  sf::RectangleShape backdrop_;
  WindowSizeButtons windowSizeButtons_;
};

}  // namespace rtype::client

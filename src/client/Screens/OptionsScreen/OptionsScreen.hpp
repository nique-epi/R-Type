#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include "IScreen.hpp"
#include "ScreenStack.hpp"
#include "WindowSizeButtons.hpp"

namespace rtype::client {

class GameWindow;

/**
 * @brief Options laid over the game: the window size buttons, on a
 * translucent layer that leaves the game visible behind.
 *
 * OPTIONS_KEY closes it and gives the events back to the game.
 *
 * Provisional: it stands in for the options menu until that one exists.
 */
class OptionsScreen : public IScreen {
 public:
  /**
   * @param screens Stack the screen lives in, which must outlive it.
   * @param window Window whose size the buttons select, which must outlive
   * the screen.
   * @param labelFont Font of the button labels, which must outlive the screen.
   */
  OptionsScreen(ScreenStack& screens, GameWindow& window,
                const sf::Font& labelFont);

  void handleEvent(const sf::Event& event) override;
  void update() override;
  void draw(sf::RenderTarget& target) const override;

 private:
  /**
   * @brief Gives the window the size whose button is under a click, when that
   * size is available.
   *
   * @param pixel Position of the click in the window, in pixels.
   */
  void selectWindowSizeAt(sf::Vector2i pixel);

  ScreenStack* screens_;
  GameWindow* window_;
  sf::RectangleShape backdrop_;
  WindowSizeButtons windowSizeButtons_;
};

}  // namespace rtype::client

#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include "IScreen.hpp"
#include "ScreenStack.hpp"

namespace rtype::client {

/**
 * @brief The game in progress; for now, the playfield background alone.
 *
 * OPTIONS_KEY puts the options screen over the game, which goes on running and
 * stays drawn behind it.
 */
class GameScreen : public IScreen {
 public:
  /**
   * @param screens Stack the screen lives in, which must outlive it.
   * @param optionsScreen Builds the screen OPTIONS_KEY puts over the game.
   */
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

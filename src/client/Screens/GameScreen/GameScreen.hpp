#pragma once

#include "IScreen.hpp"
#include "ScreenStack.hpp"

namespace rtype::client {

/**
 * @brief The game in progress; for now it draws nothing over the scrolling
 * background, which the frame loop draws under every screen.
 *
 * Releasing OPTIONS_KEY puts the options screen over the game, which goes on
 * running and stays drawn behind it. The release, not the press: the system
 * repeats the press of a held key, which would open and close the options
 * again and again.
 */
class GameScreen : public IScreen {
 public:
  /**
   * @param screens Stack the screen lives in, which must outlive it; the
   * options are pushed on it when OPTIONS_KEY is released.
   */
  explicit GameScreen(ScreenStack& screens);

  void handleEvent(const sf::Event& event) override;
  void update() override;
  void draw(sf::RenderTarget& target) const override;

 private:
  ScreenStack* screens_;
};

}  // namespace rtype::client

#pragma once

#include "TimeConstants.hpp"

namespace sf {
class Event;
class RenderTarget;
}  // namespace sf

namespace rtype::client {

/**
 * @brief One screen of the client: the game, the loading screen, a menu.
 *
 * Screens live in a ScreenStack. Only the screen on top receives the window
 * events; every screen is updated each frame, so a screen put over the game
 * leaves it running behind. Screens are drawn from the bottom up, starting at
 * the topmost screen that hides the screens below: the screens under it are
 * not drawn.
 */
class IScreen {
 public:
  virtual ~IScreen() = default;

  /** @brief Reacts to a window event; the window has already handled closing
   * and resizing. */
  virtual void handleEvent(const sf::Event& event) = 0;

  /**
   * @brief Advances the screen by one frame.
   *
   * @param elapsed Real time since the previous frame, the same for every
   * screen and for the scrolling background.
   */
  virtual void update(engine::Duration elapsed) = 0;

  /** @brief Draws the screen, in playfield units. */
  virtual void draw(sf::RenderTarget& target) const = 0;

  /**
   * @returns Whether the screens below are left undrawn while this one is in
   * the stack: true for a screen of its own, such as the options, false for a
   * dialog that dims the screen it asks about.
   */
  [[nodiscard]] virtual bool hidesScreensBelow() const = 0;
};

}  // namespace rtype::client

#pragma once

namespace sf {
class Event;
class RenderTarget;
}  // namespace sf

namespace rtype::client {

/**
 * @brief One screen of the client: the game, the loading screen, a menu.
 *
 * Screens live in a ScreenStack. Only the screen on top receives the window
 * events; every screen is updated and drawn each frame, from the bottom up, so
 * a screen put over the game leaves it running and visible behind.
 */
class IScreen {
 public:
  virtual ~IScreen() = default;

  /** @brief Reacts to a window event; the window has already handled closing
   * and resizing. */
  virtual void handleEvent(const sf::Event& event) = 0;

  /** @brief Advances the screen by one frame. */
  virtual void update() = 0;

  /** @brief Draws the screen, in playfield units. */
  virtual void draw(sf::RenderTarget& target) const = 0;
};

}  // namespace rtype::client

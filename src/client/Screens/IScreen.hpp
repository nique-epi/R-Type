#pragma once

namespace sf {
class Event;
class RenderTarget;
}  // namespace sf

namespace rtype::client {

/** @brief One screen of the client, updated and drawn each frame by the screen
 * stack. */
class IScreen {
 public:
  virtual ~IScreen() = default;

  /** @brief Reacts to a window event, received only while the screen is on
   * top. */
  virtual void handleEvent(const sf::Event& event) = 0;

  /** @brief Advances the screen by one frame. */
  virtual void update() = 0;

  /** @brief Draws the screen in playfield units. */
  virtual void draw(sf::RenderTarget& target) const = 0;
};

}  // namespace rtype::client

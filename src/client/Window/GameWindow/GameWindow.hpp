#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

namespace rtype::client {

/**
 * @brief Client window and its frame loop.
 *
 * The loop never waits on the network: it only polls window events, draws and
 * presents, at most FRAMES_PER_SECOND_LIMIT times per second. Network
 * reception is meant to run on its own thread and hand its messages over
 * through a queue the loop drains each frame.
 */
class GameWindow {
 public:
  GameWindow();

  /**
   * @brief Runs the frame loop until the window is closed.
   */
  void run();

 private:
  void handleEvents();
  void render();

  sf::RenderWindow window_;
};

}  // namespace rtype::client

#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/**
 * @brief Client window and its frame loop.
 *
 * The loop never waits on the network: it only polls window events, draws and
 * presents, at most FRAMES_PER_SECOND_LIMIT times per second. Network
 * reception is meant to run on its own thread and hand its messages over
 * through a queue the loop drains each frame.
 *
 * The window shows the whole playfield whatever its size. Drawing is done in
 * the logical units of the playfield, scaled without distortion; what the
 * playfield does not cover stays black.
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

  /**
   * @brief Makes the window show the whole playfield, undistorted and
   * centered.
   *
   * Must be called every time the size of the window changes: SFML keeps the
   * previous view otherwise, which stretches the playfield over the new size.
   *
   * @param windowSize Size of the window, in pixels.
   */
  void showWholePlayfield(sf::Vector2u windowSize);

  sf::RenderWindow window_;
  sf::RectangleShape playfieldBackground_;
};

}  // namespace rtype::client

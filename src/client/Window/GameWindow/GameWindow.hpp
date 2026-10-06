#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include "PixelSize.hpp"
#include "WindowSizeButtons.hpp"
#include "WindowSizeSelection.hpp"

namespace rtype::client {

/**
 * @brief Client window and its frame loop.
 *
 * The loop never waits on the network: it only polls window events, draws and
 * presents, at most FRAMES_PER_SECOND_LIMIT times per second. Network
 * reception is meant to run on its own thread and hand its messages over
 * through a queue the loop drains each frame.
 *
 * The window cannot be resized by dragging its border. The player picks one of
 * the sizes of WINDOW_SIZES with the buttons drawn in the playfield; the
 * window opens at the largest one that fits the desktop. Every size has the
 * proportions of the playfield, so the playfield fills the window.
 *
 * Should the system still give the window another shape, the whole playfield
 * stays visible: drawing is done in the logical units of the playfield, scaled
 * without distortion, and what the playfield does not cover stays black.
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

  /**
   * @brief Gives the window the selected size, centers it on the desktop
   * and shows the size on the buttons.
   */
  void applySelectedWindowSize();

  /**
   * @brief Selects the size whose button is under a click, when that size is
   * available.
   *
   * @param pixel Position of the click in the window, in pixels.
   */
  void selectWindowSizeAt(sf::Vector2i pixel);

  /** @returns The size of the desktop the window opens on. */
  [[nodiscard]] static PixelSize desktopSize();

  WindowSizeSelection windowSizeSelection_;
  sf::RenderWindow window_;
  sf::RectangleShape playfieldBackground_;
  WindowSizeButtons windowSizeButtons_;
};

}  // namespace rtype::client

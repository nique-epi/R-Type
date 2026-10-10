#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <optional>
#include "PixelSize.hpp"
#include "WindowSizeSelection.hpp"

namespace rtype::client {

/**
 * @brief Client window, which always shows the whole playfield.
 *
 * The window cannot be resized by dragging its border. The player picks one of
 * the sizes of WINDOW_SIZES; the window opens at the largest one that fits the
 * desktop. Every size has the proportions of the playfield, so the playfield
 * fills the window.
 *
 * Should the system still give the window another shape, the whole playfield
 * stays visible: drawing is done in the logical units of the playfield, scaled
 * without distortion, and what the playfield does not cover stays black.
 */
class GameWindow {
 public:
  /** @brief Opens the window at the selected size, centered on the desktop,
   * showing at most FRAMES_PER_SECOND_LIMIT frames per second. */
  GameWindow();

  [[nodiscard]] bool isOpen() const;

  /**
   * @brief The next event a screen may react to.
   *
   * The window handles closing and resizing itself and never returns those
   * events: closing closes the window, resizing keeps the whole playfield in
   * view.
   *
   * @returns The event, or an empty optional once no event is left or the
   * window is closed.
   */
  [[nodiscard]] std::optional<sf::Event> pollScreenEvent();

  /** @brief Clears the window to black before a frame is drawn. */
  void clear();

  /** @brief Where a frame is drawn, in playfield units. */
  [[nodiscard]] sf::RenderTarget& renderTarget();

  /** @brief Shows what was drawn since clear(). */
  void display();

  [[nodiscard]] const WindowSizeSelection& windowSizeSelection() const;

  /**
   * @brief Gives the window the size at this index of WINDOW_SIZES, centered
   * on the desktop.
   *
   * @returns false, changing nothing, when that size is not available.
   */
  bool selectWindowSize(std::size_t index);

  /**
   * @param pixel Position in the window, in pixels.
   * @returns The point of the playfield under that pixel, in playfield units.
   */
  [[nodiscard]] sf::Vector2f playfieldPointAt(sf::Vector2i pixel) const;

 private:
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

  /** @brief Gives the window the selected size and centers it on the
   * desktop. */
  void applySelectedWindowSize();

  /** @returns The size of the desktop the window opens on. */
  [[nodiscard]] static PixelSize desktopSize();

  WindowSizeSelection windowSizeSelection_;
  sf::RenderWindow window_;
};

}  // namespace rtype::client

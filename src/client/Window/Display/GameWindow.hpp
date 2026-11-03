#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <optional>
#include "DisplayMode.hpp"
#include "ISizableWindow.hpp"
#include "PixelSize.hpp"
#include "WindowLayout.hpp"

namespace rtype::client {

/**
 * @brief Client window, which always shows the whole playfield.
 *
 * Building it opens nothing: open() opens the window, close() closes it, so
 * the module that owns it builds it while it installs and opens it when it
 * starts, once the settings that give its size are read.
 *
 * The window cannot be resized by dragging its border. Windowed, the player
 * picks one of the sizes of WINDOW_SIZES; the window opens at the largest one
 * that fits the desktop, or at the size the player saved when it still fits.
 * Every size has the proportions of the playfield, so the playfield fills the
 * window. Borderless, the window covers the whole screen at the size of the
 * desktop, without changing its resolution.
 *
 * Changing the display mode closes the window and opens it again in the other
 * mode: SFML 3 changes the border of a window only when it opens it.
 *
 * Should the system still give the window another shape, or the desktop not
 * have the proportions of the playfield, the whole playfield stays visible:
 * drawing is done in the logical units of the playfield, scaled without
 * distortion, and what the playfield does not cover stays black.
 */
class GameWindow : public ISizableWindow {
 public:
  /** @brief A closed window, laid out windowed at the largest size the
   * desktop holds until open() lays it out again. */
  GameWindow();

  /**
   * @brief Opens the window in the given display mode; windowed, at the
   * preferred size when it is available, at the largest available one
   * otherwise, centered on the desktop. It shows at most
   * FRAMES_PER_SECOND_LIMIT frames per second. A window already open is
   * closed first.
   */
  void open(std::optional<PixelSize> preferredSize, DisplayMode displayMode);

  /** @brief Closes the window; does nothing when it is closed. */
  void close();

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

  /** @brief Where a frame is drawn, in playfield units; its clear() clears
   * the window to black before a frame is drawn. */
  [[nodiscard]] sf::RenderTarget& renderTarget();

  /** @brief Shows what was drawn since the window was cleared. */
  void display();

  [[nodiscard]] const WindowLayout& windowLayout() const override;

  /**
   * @brief Gives the window the size at this index of WINDOW_SIZES, centered on
   * the desktop.
   *
   * @returns false, changing nothing, when the window is borderless or that
   * size is not available.
   */
  bool selectWindowSize(std::size_t index) override;

  /**
   * @brief Closes the window and opens it again in this display mode.
   *
   * @returns false, changing nothing, when the window is already in it.
   */
  bool selectDisplayMode(DisplayMode mode) override;

  /**
   * @param pixel Position in the window, in pixels.
   * @returns The point of the playfield under that pixel, in playfield units.
   */
  [[nodiscard]] sf::Vector2f playfieldPointAt(sf::Vector2i pixel) const;

 private:
  WindowLayout layout_;
  sf::RenderWindow window_;
};

}  // namespace rtype::client

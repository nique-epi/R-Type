#pragma once

#include <cstddef>
#include <optional>
#include "DisplayMode.hpp"
#include "PixelSize.hpp"
#include "WindowFrame.hpp"
#include "WindowSizeSelection.hpp"

namespace rtype::client {

/**
 * @brief How the window lies on a given desktop: its display mode and, when
 * windowed, which of the sizes of WINDOW_SIZES it takes.
 *
 * Windowed, the window takes the selected size, centered on the desktop.
 * Borderless, it covers the whole desktop from its top-left corner, whatever
 * size is selected; the selected size is kept, and comes back once the window
 * is windowed again. A size is selected only while the window is windowed.
 */
class WindowLayout {
 public:
  /**
   * @param desktop Size of the desktop the window opens on.
   * @param displayMode Display mode the window opens in.
   * @param preferredSize Size the player saved, selected when it is available;
   * the largest available size is selected otherwise.
   */
  WindowLayout(PixelSize desktop, DisplayMode displayMode,
               std::optional<PixelSize> preferredSize);

  [[nodiscard]] const WindowSizeSelection& sizes() const;
  [[nodiscard]] DisplayMode displayMode() const;

  /**
   * @returns The size the display mode gives the window whatever size is
   * selected: the size of the desktop, borderless; nothing, windowed.
   */
  [[nodiscard]] std::optional<PixelSize> imposedSize() const;

  /** @returns Where the window lies in its display mode. */
  [[nodiscard]] WindowFrame frame() const;

  /**
   * @brief Makes the size at this index of WINDOW_SIZES the selected one.
   * @returns false, changing nothing, when the window is borderless or that
   * size is not available.
   */
  bool selectSize(std::size_t index);

  /**
   * @brief Puts the window in this display mode.
   * @returns false, changing nothing, when the window is already in it.
   */
  bool selectDisplayMode(DisplayMode mode);

 private:
  PixelSize desktop_;
  WindowSizeSelection sizes_;
  DisplayMode displayMode_ = DisplayMode::Windowed;
};

}  // namespace rtype::client

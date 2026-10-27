#pragma once

#include <cstddef>
#include "DisplayMode.hpp"

namespace rtype::client {

class WindowLayout;

/** @brief A window whose display mode, and size among WINDOW_SIZES, the player
 * picks. */
class ISizableWindow {
 public:
  virtual ~ISizableWindow() = default;

  /** @returns The display mode of the window, which sizes fit the desktop, and
   * which one the window has. */
  [[nodiscard]] virtual const WindowLayout& windowLayout() const = 0;

  /** @brief Gives the window the size at this index of WINDOW_SIZES.
   * @returns false, changing nothing, when the window is borderless or that
   * size is not available. */
  virtual bool selectWindowSize(std::size_t index) = 0;

  /** @brief Puts the window in this display mode.
   * @returns false, changing nothing, when the window is already in it. */
  virtual bool selectDisplayMode(DisplayMode mode) = 0;
};

}  // namespace rtype::client

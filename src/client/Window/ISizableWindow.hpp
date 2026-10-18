#pragma once

#include <cstddef>

namespace rtype::client {

class WindowSizeSelection;

/** @brief A window whose size the player picks among WINDOW_SIZES. */
class ISizableWindow {
 public:
  virtual ~ISizableWindow() = default;

  /** @returns Which sizes fit the desktop, and which one the window has. */
  [[nodiscard]] virtual const WindowSizeSelection& windowSizeSelection()
      const = 0;

  /** @brief Gives the window the size at this index of WINDOW_SIZES.
   * @returns false, changing nothing, when that size is not available. */
  virtual bool selectWindowSize(std::size_t index) = 0;
};

}  // namespace rtype::client

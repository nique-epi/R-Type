#pragma once

#include <cstddef>
#include "PixelPosition.hpp"
#include "PixelSize.hpp"

namespace rtype::client {

/**
 * @brief Which of the sizes of WINDOW_SIZES the window uses, on a given
 * desktop.
 *
 * A size is available only when it is strictly smaller than the desktop in
 * both directions: a window as large as the desktop does not fit once its
 * title bar is added. The largest available size is selected at construction,
 * and the smallest size of the list when none is available.
 */
class WindowSizeSelection {
 public:
  /** @param desktop Size of the desktop the window opens on. */
  explicit WindowSizeSelection(PixelSize desktop);

  /**
   * @returns true when the size at this index of WINDOW_SIZES fits the
   * desktop; false when it does not or when the index is outside the list.
   */
  [[nodiscard]] bool isAvailable(std::size_t index) const;

  /**
   * @brief Makes the size at this index of WINDOW_SIZES the selected one.
   * @returns false, leaving the selection unchanged, when that size is not
   * available.
   */
  bool select(std::size_t index);

  [[nodiscard]] std::size_t selectedIndex() const;
  [[nodiscard]] PixelSize selected() const;

  /**
   * @returns Where to put a window of the selected size so it is centered on
   * the desktop. Along an axis where the window is larger than the desktop,
   * the window starts at the edge of the desktop.
   */
  [[nodiscard]] PixelPosition centeredPosition() const;

 private:
  /**
   * @returns Half of the space a window leaves free along one axis of the
   * desktop, or nothing when the window is not smaller than the desktop.
   */
  [[nodiscard]] static int centeredOffset(unsigned int desktopExtent,
                                          unsigned int windowExtent);

  PixelSize desktop_;
  std::size_t selectedIndex_ = 0;
};

}  // namespace rtype::client

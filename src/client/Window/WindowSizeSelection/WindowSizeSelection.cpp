#include "WindowSizeSelection.hpp"
#include <cstddef>
#include "PixelPosition.hpp"
#include "PixelSize.hpp"
#include "WindowConstants.hpp"

namespace rtype::client {

WindowSizeSelection::WindowSizeSelection(PixelSize desktop)
    : desktop_(desktop) {
  for (std::size_t index = 0; index < WINDOW_SIZES.size(); ++index) {
    if (isAvailable(index)) {
      selectedIndex_ = index;
    }
  }
}

bool WindowSizeSelection::isAvailable(std::size_t index) const {
  return index < WINDOW_SIZES.size() &&
         WINDOW_SIZES[index].width < desktop_.width &&
         WINDOW_SIZES[index].height < desktop_.height;
}

bool WindowSizeSelection::select(std::size_t index) {
  if (!isAvailable(index)) {
    return false;
  }
  selectedIndex_ = index;
  return true;
}

std::size_t WindowSizeSelection::selectedIndex() const {
  return selectedIndex_;
}

PixelSize WindowSizeSelection::selected() const {
  return WINDOW_SIZES[selectedIndex_];
}

int WindowSizeSelection::centeredOffset(unsigned int desktopExtent,
                                        unsigned int windowExtent) {
  if (windowExtent >= desktopExtent) {
    return 0;
  }
  return static_cast<int>((desktopExtent - windowExtent) /
                          SIDES_SHARING_FREE_SPACE);
}

PixelPosition WindowSizeSelection::centeredPosition() const {
  const PixelSize size = selected();
  return {.x = centeredOffset(desktop_.width, size.width),
          .y = centeredOffset(desktop_.height, size.height)};
}

}  // namespace rtype::client

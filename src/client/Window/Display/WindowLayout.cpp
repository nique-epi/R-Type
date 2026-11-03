#include "WindowLayout.hpp"
#include <cstddef>
#include <optional>
#include "DisplayMode.hpp"
#include "PixelSize.hpp"
#include "WindowConstants.hpp"
#include "WindowFrame.hpp"
#include "WindowSizeSelection.hpp"

namespace rtype::client {

WindowLayout::WindowLayout(PixelSize desktop, DisplayMode displayMode,
                           std::optional<PixelSize> preferredSize)
    : desktop_(desktop), sizes_(desktop), displayMode_(displayMode) {
  if (preferredSize.has_value()) {
    sizes_.selectSize(*preferredSize);
  }
}

const WindowSizeSelection& WindowLayout::sizes() const { return sizes_; }

DisplayMode WindowLayout::displayMode() const { return displayMode_; }

std::optional<PixelSize> WindowLayout::imposedSize() const {
  if (displayMode_ == DisplayMode::Borderless) {
    return desktop_;
  }
  return std::nullopt;
}

WindowFrame WindowLayout::frame() const {
  const std::optional<PixelSize> imposed = imposedSize();
  if (imposed.has_value()) {
    return WindowFrame{.size = *imposed, .position = DESKTOP_CORNER};
  }
  return WindowFrame{.size = sizes_.selected(),
                     .position = sizes_.centeredPosition()};
}

bool WindowLayout::selectSize(std::size_t index) {
  if (imposedSize().has_value()) {
    return false;
  }
  return sizes_.select(index);
}

bool WindowLayout::selectDisplayMode(DisplayMode mode) {
  if (mode == displayMode_) {
    return false;
  }
  displayMode_ = mode;
  return true;
}

}  // namespace rtype::client

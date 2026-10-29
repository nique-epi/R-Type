#include "FakeSizableWindow.hpp"
#include <cstddef>
#include <optional>
#include "DisplayMode.hpp"
#include "PixelSize.hpp"
#include "WindowLayout.hpp"

FakeSizableWindow::FakeSizableWindow(rtype::client::PixelSize desktop)
    : layout_(desktop, rtype::client::DisplayMode::Windowed, std::nullopt) {}

const rtype::client::WindowLayout& FakeSizableWindow::windowLayout() const {
  return layout_;
}

bool FakeSizableWindow::selectWindowSize(std::size_t index) {
  return layout_.selectSize(index);
}

bool FakeSizableWindow::selectDisplayMode(rtype::client::DisplayMode mode) {
  return layout_.selectDisplayMode(mode);
}

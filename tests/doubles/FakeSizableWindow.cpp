#include "FakeSizableWindow.hpp"
#include <cstddef>
#include "PixelSize.hpp"
#include "WindowSizeSelection.hpp"

FakeSizableWindow::FakeSizableWindow(rtype::client::PixelSize desktop)
    : selection_(desktop) {}

const rtype::client::WindowSizeSelection&
FakeSizableWindow::windowSizeSelection() const {
  return selection_;
}

bool FakeSizableWindow::selectWindowSize(std::size_t index) {
  return selection_.select(index);
}

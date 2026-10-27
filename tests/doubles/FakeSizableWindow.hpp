#pragma once

#include <cstddef>
#include "DisplayMode.hpp"
#include "ISizableWindow.hpp"
#include "PixelSize.hpp"
#include "WindowLayout.hpp"

/** @brief A window on a desktop of a given size, windowed at first, as
 * WindowLayout sees it; it opens no window. */
class FakeSizableWindow : public rtype::client::ISizableWindow {
 public:
  explicit FakeSizableWindow(rtype::client::PixelSize desktop);

  [[nodiscard]] const rtype::client::WindowLayout& windowLayout()
      const override;
  bool selectWindowSize(std::size_t index) override;
  bool selectDisplayMode(rtype::client::DisplayMode mode) override;

 private:
  rtype::client::WindowLayout layout_;
};

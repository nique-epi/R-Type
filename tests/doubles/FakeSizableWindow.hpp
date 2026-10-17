#pragma once

#include <cstddef>
#include "ISizableWindow.hpp"
#include "PixelSize.hpp"
#include "WindowSizeSelection.hpp"

/** @brief A window on a desktop of a given size, as WindowSizeSelection sees
 * it; it opens no window. */
class FakeSizableWindow : public rtype::client::ISizableWindow {
 public:
  explicit FakeSizableWindow(rtype::client::PixelSize desktop);

  [[nodiscard]] const rtype::client::WindowSizeSelection& windowSizeSelection()
      const override;
  bool selectWindowSize(std::size_t index) override;

 private:
  rtype::client::WindowSizeSelection selection_;
};
